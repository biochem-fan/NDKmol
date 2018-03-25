/*  NDKmol - Molecular Viewer for Android

     (C) Copyright 2011 - 2012, biochem_fan

     This file is part of NDKmol.

     NDKmol is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>. */

package jp.sfjp.webglmol.NDKmol;

import android.app.Activity;
import android.app.TabActivity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TabHost;

public class SearcherTab extends TabActivity {
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.searchertab);    
		initTabs();
	}

	public void finishFromChild(Activity child) {
		// Pass-through result from child using child's intent (not child's result intent)
		if (child.getIntent().getDataString() != null) {
			Intent i = child.getIntent();
			setResult(RESULT_OK, i);
		} else {
			setResult(RESULT_CANCELED);
		}
		super.finishFromChild(child); 
	} 

	protected void initTabs(){
		TabHost tabHost = getTabHost();
		TabHost.TabSpec spec;
		Intent intent;

		intent = new Intent().setClass(this, PDBSearcher.class);
		spec = tabHost.newTabSpec("Tab1").setIndicator("PDB").setContent(intent);
		tabHost.addTab(spec);

		intent = new Intent().setClass(this, PubChemSearcher.class);
		spec = tabHost.newTabSpec("Tab2").setIndicator("PubChem").setContent(intent);
		tabHost.addTab(spec);

		intent = new Intent().setClass(this, CACTUSSearcher.class);
		spec = tabHost.newTabSpec("Tab2").setIndicator("NCI CACTUS").setContent(intent);
		tabHost.addTab(spec);
		
		tabHost.setCurrentTab(0);
	}
}
