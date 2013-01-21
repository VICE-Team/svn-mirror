package org.ab.uae;

import com.locnet.vice.R;

import java.io.File;
import java.util.ArrayList;

import org.ab.nativelayer.ImportFileView;

import android.app.Application;

public class RomImportView extends ImportFileView {

	public RomImportView() {
		super(new String [] {} );
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
