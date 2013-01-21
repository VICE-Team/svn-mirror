/*
 *  Copyright (C) 2011 Locnet (android.locnet@gmail.com)
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

package com.locnet.vice;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

public class DosBoxAudio
{
	private DosBoxLauncher mParent = null;
	private boolean mAudioRunning=true;
	private AudioTrack mAudio = null;
	private int mChannelCount;

	public short[] mAudioBuffer = null;	
	
	private DosBoxAudioThread mAudioThread = null;
	
	class DosBoxAudioThread extends Thread {
		//private long nextAudioUpdateTime = 0;
		//private int audioResetCounter = 0;
		//public Boolean mWritingBuffer = false;	//2011-09-07, try fixing bug on loading savestate
		
		public void run() {
			//Log.d("c64", "audioThread:run");
			while (mAudioRunning) {
				if (mParent.mDosBoxThread.mDosBoxRunning && (mParent.mAudioDevice.mAudioBuffer != null)) {
					long now = System.currentTimeMillis();
					//if (nextAudioUpdateTime == 0)
					//	nextAudioUpdateTime = now;
					/*if ((now - nextAudioUpdateTime) > mParent.mAudioDevice.mAudioMinUpdateInterval) {
						nextAudioUpdateTime = now;
						audioResetCounter = 0;
					}*/
					
					//while (now >= nextAudioUpdateTime) {
					//if (now >= nextAudioUpdateTime + (audioResetCounter >> 2)) {
					//if (now >= nextAudioUpdateTime) {
					boolean bufferUnderRun = false;
					synchronized(mAudioBuffer) {						
						//mWritingBuffer = true;
						int size = DosBoxLauncher.nativeAudioWriteBuffer(mParent.mAudioDevice.mAudioBuffer);
						if (size > 0) {
							//Log.d("c64", "write:" + size);
							mParent.callbackAudioWriteBuffer(size);
						}
						else {
							bufferUnderRun = true;
						}
						//mWritingBuffer = false;
					}
					if (bufferUnderRun) {
						try {
							Thread.sleep(10);
						} catch (InterruptedException e) {
						}
						continue;
						//	Log.d("c64", "write:underrun");
					}
						/*nextAudioUpdateTime += mParent.mAudioDevice.mAudioMinUpdateInterval;
						audioResetCounter++;*/
					//}
					
					/*if (audioResetCounter >= 100) {
						audioResetCounter = 0;
						long resetAudioUpdateTime = now + mParent.mAudioDevice.mAudioMinUpdateInterval;
						//if (Math.abs(resetAudioUpdateTime - nextAudioUpdateTime) > 2*mParent.mAudioDevice.mAudioMinUpdateInterval)
							nextAudioUpdateTime = resetAudioUpdateTime;
					}*/
					try {
						//long sleepTime = nextAudioUpdateTime - System.currentTimeMillis() + (audioResetCounter >> 2);
						//long sleepTime = nextAudioUpdateTime - System.currentTimeMillis();
						long sleepTime = (mParent.mAudioDevice.mAudioMinUpdateInterval - (System.currentTimeMillis() - now))/2;
						if (sleepTime < 10)
							sleepTime = 10;
						//Log.d("c64", "audio sleep:" + sleepTime + "," + nextAudioUpdateTime + "," + System.currentTimeMillis());
						//if (sleepTime > 0)
							Thread.sleep(sleepTime);
					} catch (InterruptedException e) {
					}
				}
				else {
					try {
						/*audioResetCounter = 0;
						nextAudioUpdateTime = 0;*/
						Thread.sleep(1000);
					} catch (InterruptedException e) {
					}					
				}
			}				
			//Log.d("c64", "audioThread:stop");
			/*nextAudioUpdateTime = 0;
			audioResetCounter = 0;*/
		}
	}
	
	DosBoxAudio(DosBoxLauncher context) {
		mParent = context;
		mAudioThread = new DosBoxAudioThread();
	}

	public int initAudio(int rate, int channels, int encoding, int bufSize)
	{
		//Log.d("c64", "initAudio:" + rate + "," + channels + "," + bufSize);
		if (mAudio != null) {
			synchronized(mAudioBuffer) {	//2011-09-07, try fixing bug on loading savestate
				/*while (mAudioThread.mWritingBuffer) {
					try {
						Thread.sleep(5);
					} catch (InterruptedException e) {
					}
				}*/
				//Log.d("c64", "initAudio:" + mAudioThread.mWritingBuffer);
				mAudio.release();
				mAudio = null;
				mAudioBuffer = null;
			}
		}
		if( mAudio == null )
		{
			int bufSize2 = bufSize;

			channels = ( channels == 1 ) ? AudioFormat.CHANNEL_CONFIGURATION_MONO : 
											AudioFormat.CHANNEL_CONFIGURATION_STEREO;
			encoding = ( encoding == 1 ) ? AudioFormat.ENCODING_PCM_16BIT :
											AudioFormat.ENCODING_PCM_8BIT;

			if( AudioTrack.getMinBufferSize( rate, channels, encoding ) > bufSize ) {
				//bufSize = AudioTrack.getMinBufferSize( rate, channels, encoding );
				bufSize2 = AudioTrack.getMinBufferSize( rate, channels, encoding );
				//bufSize2 = Math.max(bufSize2, bufSize * 2);
				bufSize2 = Math.max(bufSize2, bufSize << 3);
			}
			//mAudioTrackBufSample = (bufSize2 >> ((encoding == AudioFormat.ENCODING_PCM_16BIT)?1:0)) >> ((channels == AudioFormat.CHANNEL_CONFIGURATION_STEREO)?1:0);
			//mAudioTrackBufSampleTime = 1000*mAudioTrackBufSample/rate;
			
			mAudioMinUpdateInterval = 1000*(bufSize >> 1)/((channels == AudioFormat.CHANNEL_CONFIGURATION_MONO)?1:2)/rate;
			
			mAudioBuffer = new short[bufSize >> 1];
			mAudio = new AudioTrack(AudioManager.STREAM_MUSIC,
										rate,
										channels,
										encoding,
										//bufSize,
										bufSize2,
										AudioTrack.MODE_STREAM );
			mAudio.pause();

			mChannelCount = mAudio.getChannelCount();

			if (!mAudioThread.isAlive()) {
				//mAudioThread.setPriority(Thread.MAX_PRIORITY);
				mAudioThread.start();
			}
				

			return bufSize;
		}
		if (mAudioBuffer != null)
			return bufSize;
		else
			return 0;
	}
   
	public void shutDownAudio() {
	   if (mAudio != null) {
		   mAudio.stop();
		   mAudio.release();
		   mAudio = null;
	   }
	   mAudioBuffer = null;	
	}

	private long mLastWriteBufferTime = 0;	
	public int mAudioMinUpdateInterval = 50;

	public void AudioWriteBuffer(int size) {
		if ((mAudioBuffer != null) && mAudioRunning) {
			long now = System.currentTimeMillis();
			if ((!mParent.mTurboOn) || ((now - mLastWriteBufferTime) > mAudioMinUpdateInterval)) {
				if (size > 0)
					writeSamples( mAudioBuffer, size * mChannelCount);
				mLastWriteBufferTime = now;
			}
		}
	}
   
	public void setRunning() {
		mAudioRunning = !mAudioRunning;
		if (!mAudioRunning)
			mAudio.pause();
		
		if (mAudioRunning)
			mAudioThread.start();
	}
   
	//int totalSize = 0;
	//long lastTime = 0;
	//int mAudioTrackBufSample;
	//int mAudioTrackBufSampleTime;
	//boolean skip = false;
	
   public void writeSamples(short[] samples, int size) 
   {
	   if (mAudioRunning) {
	      if (mAudio != null) {
//Log.d("c64", "writeSamples:" + size);
	    	  {
		    	  //if (totalSize >= (mAudioTrackBufSample)) {
	    		  /*if (totalSize >= (220500)) {
		    		  long now = System.currentTimeMillis();
	    			  Log.d("c64", "writeSamples:" + size + "," + mAudioTrackBufSample + "," + totalSize*1000/(System.currentTimeMillis() - lastTime));
		    		  if ((now - lastTime) < mAudioTrackBufSampleTime) {
			    		  //Log.d("c64", "writeSamples:overrun:" + (now - lastTime) + "," + mAudioTrackBufSample + "," + mAudioTrackBufSampleTime);
		    			  //skip = true;
		    		  }
		    		  lastTime = now;
		    		  totalSize = 0;
		    	  }*/
	    	  }	    	  
			  mAudio.write( samples, 0, size);
	    	  /*if (!skip) {
	    		  {
	    			  long now = System.currentTimeMillis();
	    			  mAudio.write( samples, 0, size);
	    			  if ((System.currentTimeMillis() - now) > 10) {
			    		  Log.d("c64", "writeSamples:delay:" + size + "," + (System.currentTimeMillis() - now));
	    				  //skip = true;
	    			  }
	    		  }
		    	  totalSize += size;
	    	  }
	    	  else
	    		  skip = false;*/

	    	  if (mAudio.getPlayState() != AudioTrack.PLAYSTATE_PLAYING)
	    		  play();
	      }
	   }
   }

   public void play() {
	   if (mAudio != null)
		   mAudio.play();
   }
   
   public void pause() {
	   if (mAudio != null)
		   mAudio.pause();
   }   
}

