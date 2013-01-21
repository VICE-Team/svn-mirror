package org.ab.uae;

import com.locnet.vice.R;

import java.io.File;
import java.util.ArrayList;

import org.ab.nativelayer.ImportFileView;

import android.app.Application;

public class FloppyImportView extends ImportFileView {

	public FloppyImportView() {
		super(new String [] { "d64", "d71", "d81", "g64", "d80", "d82", "prg", "p00", "t64", "tap" } );
		virtualDir = false;
	}

	private static final long serialVersionUID = -8756086087950123786L;

	@Override
	public ArrayList<String> getExtendedList(Application application, File file) {
		return null;
	}

	@Override
	public String getExtra2(int position) {
		return null;
	}

	@Override
	public int getIcon(int position) {
		return R.drawable.file;
	}

}
