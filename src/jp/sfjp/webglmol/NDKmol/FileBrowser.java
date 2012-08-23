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

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.AdapterView.OnItemClickListener;

public class FileBrowser extends Activity {

	private ListView listView = null;
	private List<Map<String, String>> dataList;
	private FileBrowser self;
	private String currentPath;


	private List<Map<String,String>> getFileList(String path) {
		List<Map<String,String>> ret = new ArrayList<Map<String,String>>();

		File dir = new File(path);
		File[] files = dir.listFiles();
		Log.d("FileBrowser", dir.toURI().toString());

		for (int i = 0, lim = files.length; i < lim; i++) {
			String	name = "", title = ""; 
			try {
				HashMap<String, String> records = new HashMap<String, String>();
				File file = files[i];
				name = file.getName();
				String upperCased = name.toUpperCase();
				if (upperCased.endsWith("PDB")) {
					FileInputStream input = new FileInputStream(file);
					InputStreamReader reader = new InputStreamReader(input);
					int headerLength = 300;
					char buffer[] = new char[headerLength];
					reader.read(buffer, 0, headerLength);
					String header[] = new String(buffer).split("\n");
					for (int j = 0; j < header.length; j++) {
						if (header[j].startsWith("TITLE") && header[j].length() > 11) {
							title += header[j].substring(10).replace("  ", "");
						}
					}
				} else if (upperCased.endsWith("SDF") || upperCased.endsWith("MOL")) {
					title = "a SDF/MOL file";
				} else {
					continue;
				}

				records.put("fileName", name);
				records.put("structureTitle", title);
				ret.add(records);
			} catch (Exception e) {

			}
		}
		return ret;
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		self = this;
		setContentView(R.layout.filebrowser);
		String tmp = getIntent().getDataString();
		Log.d("FileBrowser", tmp);
		currentPath = tmp.substring(7) + "/"; // file:// TODO: Error handling

		listView = (ListView)findViewById(R.id.searchResults);
		dataList = null;
//		searchButton = (Button)findViewById(R.id.searchButton);
//		keyword = (EditText)findViewById(R.id.keyword);
//		searchButton.setOnClickListener(new OnClickListener() {
//			public void onClick(View v) {
//			}
//		});

		listView.setOnItemClickListener(new OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View view, int position,
					long id) {
				Map<String, String> item = (Map<String, String>) listView.getItemAtPosition(position);
				Log.d("FileBrowser", "Selected " + currentPath + item.get("fileName"));
				Intent i = new Intent();
				setResult(RESULT_OK, i);
				i.setData(Uri.parse("file://" + currentPath + item.get("fileName")));
				finish();
			}
		});

		dataList = getFileList(currentPath);
		SimpleAdapter adapter = new SimpleAdapter(
				self,
				dataList,
				android.R.layout.simple_list_item_2,
				new String[] { "fileName", "structureTitle"},
				new int[] { android.R.id.text1, android.R.id.text2 }
		);
		listView.setAdapter(adapter);
	}
}