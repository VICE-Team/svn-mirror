package org.ab.nativelayer;

import java.io.File;
import java.io.Serializable;
import java.util.ArrayList;

import android.app.Application;

public abstract class ImportFileView implements Serializable {
	
	private static final long serialVersionUID = 6009558385710619718L;
	private ArrayList<String> extensions;
	protected boolean virtualDir = true;
	
	public ImportFileView(String ext []) {
		if (ext != null && ext.length > 0) {
			extensions = new ArrayList<String>();
			for(int i=0;i<ext.length;i++)
				extensions.add(ext[i]);
		}
	}
	
	public ArrayList<String> getExtensions() {
		return extensions;
	}
	
	public abstract ArrayList<String> getExtendedList(Application application, File file);
	
	public abstract String getExtra2(int position);
	
	public abstract int getIcon(int position);

}
