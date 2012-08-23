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

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.net.SocketAddress;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.DialogInterface.OnShowListener;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class PDBSearcher extends Activity {

	private ListView listView = null;
	private Button searchButton;
	private EditText keyword;
	private String pdbSearchString = "<orgPdbQuery><queryType>org.pdb.query.simple.MoleculeNameQuery</queryType><macromoleculeName>#KEYWORD#</macromoleculeName></orgPdbQuery>";
	private String pdbRestURI = "http://www.rcsb.org/pdb/rest/search/";
	private String pdbDetailSearchURI = "http://www.rcsb.org/pdb/rest/customReport?pdbids=#PDBID#&customReportColumns=structureId,structureTitle,experimentalTechnique,depositionDate,releaseDate,ndbId,resolution,structureAuthor&format=xml";
	private List<Map<String, String>> dataList;
	private int MAXRESULT = 100;
	private PDBSearcher self;
	private View detailsView;
	private Proxy proxy;

	private ArrayList<String> queryPDBforIDs(String keyword) {
		ArrayList<String> ids = new ArrayList<String>();

		if (keyword.matches("^[a-zA-Z0-9]{4}$")) { // seems to be PDBID. 
			ids.add(keyword); // non-existing PDB ID is simply ignored so try adding it.
		}

		try {
			URL url = new URL(pdbRestURI);
			HttpURLConnection conn;
			if (proxy != null) {
				conn = (HttpURLConnection) url.openConnection(proxy);
			} else {
				conn = (HttpURLConnection) url.openConnection();
			}
			conn.setDoOutput(true);

			OutputStream os = conn.getOutputStream();
			String queryStr = pdbSearchString.replaceFirst("#KEYWORD#", keyword);
			PrintStream ps = new PrintStream(os);
			ps.print(queryStr);
			ps.close();

			InputStream is = conn.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			String line;
			int cnt = 0;
			while ((line = reader.readLine()) != null) {
				ids.add(line);
				if (cnt++ > MAXRESULT) {
					Toast.makeText(this, getString(R.string.tooManyHits), Toast.LENGTH_LONG).show();
					break; 
				}
			}
			reader.close();
		} catch (Exception e) {
			Log.d("queryPDB", e.toString());
		}
		return ids;
	}

	private List<Map<String,String>> queryPDBforDetails(ArrayList<String> ids) {
		List<Map<String,String>> ret = new ArrayList<Map<String,String>>();
		String joined = "";
		StringBuffer sb = new StringBuffer();

		int lim = ids.size();
		if (lim > MAXRESULT) lim = MAXRESULT;
		for (int i = 0; i < lim; i++) joined += ids.get(i) + ","; // final ',' doesn't harm

		try {
			URL url = new URL(pdbDetailSearchURI.replaceFirst("#PDBID#", joined));
			HttpURLConnection conn;
			if (proxy != null) {
				conn = (HttpURLConnection) url.openConnection(proxy);
			} else {
				conn = (HttpURLConnection) url.openConnection();
			}

			InputStream is = conn.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			String line;

			while ((line = reader.readLine()) != null) {
				sb.append(line);
			}
			reader.close();
		} catch (Exception e) {
			Log.d("queryPDB", e.toString());
		}

		String[] entries = sb.toString().split("</record>");
		String[] fields = {"structureId", "structureTitle", "resolution", "structureAuthor", "releaseDate"};
		for (int i = 0, ilim = entries.length; i < ilim; i++) {
			String entry = entries[i];
			HashMap<String, String> records = new HashMap<String, String>();
			for (int j = 0, jlim = fields.length; j < jlim; j++) {
				String startTag = "<" + fields[j] + ">";
				String endTag = "</" + fields[j] + ">";
				int lindex = entry.indexOf(startTag);
				int rindex = entry.indexOf(endTag);
				if (lindex < 0 || rindex < 0) continue;
				lindex += startTag.length();
				String data = entry.substring(lindex, rindex);
				records.put(fields[j], data);
			}
			if (records.containsKey("structureId")) ret.add(records);
		}

		return ret;
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		self = this;
		setContentView(R.layout.searcher);

		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(self);
		if (prefs.getBoolean(self.getString(R.string.useProxy), false)) {
			String proxyAddress = prefs.getString(self.getString(R.string.proxyHost), "");
			int proxyPort = Integer.parseInt(prefs.getString(self.getString(R.string.proxyPort), "8080"));

			SocketAddress addr = new InetSocketAddress(proxyAddress, proxyPort);
			proxy = new Proxy(Proxy.Type.HTTP, addr);
		} else {
			proxy = null;
		}

		listView = (ListView)findViewById(R.id.searchResults);
		searchButton = (Button)findViewById(R.id.searchButton);
		keyword = (EditText)findViewById(R.id.keyword);
		searchButton.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				String[] tmp = {keyword.getText().toString()};
				new SearchTask().execute(tmp);
			}
		});

		listView.setOnItemClickListener(new OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View view, int position,
					long id) {
				Map<String, String> item = (Map<String, String>) listView.getItemAtPosition(position);
				final String PDBid = item.get("structureId");
				// TODO: 無駄が多い。removeView 使えば再利用できるようになる?
				LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
				detailsView = inflater.inflate(R.layout.detailview, null);

				((TextView)detailsView.findViewById(R.id.textID)).setText(PDBid);
				((TextView)detailsView.findViewById(R.id.textTitle)).setText(item.get("structureTitle"));
				String resolution = item.get("resolution");
				if (resolution != null && !resolution.equals("null")) { // NMR structures
					((TextView)detailsView.findViewById(R.id.textResolution)).setText(item.get("resolution"));
				} else {
					((TextView)detailsView.findViewById(R.id.textResolution)).setText("N/A");
				}
				((TextView)detailsView.findViewById(R.id.textAuthors)).setText(item.get("structureAuthor"));
				((TextView)detailsView.findViewById(R.id.textReleaseDate)).setText(item.get("releaseDate"));
				Builder b = new AlertDialog.Builder(self)
				.setIcon(android.R.drawable.ic_dialog_info)
				.setTitle("Structure details")
				.setView(detailsView)
				.setPositiveButton("Download", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						Intent i = new Intent();
						i.setData(Uri.parse("http://www.pdb.org/pdb/files/" + PDBid.toUpperCase() + ".pdb"));
						setResult(RESULT_OK, i);
						getIntent().setData(i.getData());
						finish();
					}
				})
				.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
					}
				});

				b.show();
			}
		});
	}

	public class SearchTask extends AsyncTask<String, Integer, Boolean> {
		ProgressDialog progress;

		@Override
		protected void onPreExecute() {
			progress = new ProgressDialog(PDBSearcher.this);

			progress.setTitle(PDBSearcher.this.getString(R.string.searching));
			progress.setMessage(PDBSearcher.this.getString(R.string.pleaseWait));
			progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
			progress.show();
		}

		@Override
		protected Boolean doInBackground(String... searchFor) {
			ArrayList<String> ids = queryPDBforIDs(searchFor[0]);
			dataList = queryPDBforDetails(ids);
			return true;
		}

		@Override
		protected void onPostExecute(Boolean result) {
			SimpleAdapter adapter = new SimpleAdapter(
					self,
					dataList,
					android.R.layout.simple_list_item_2,
					new String[] { "structureId", "structureTitle"},
					new int[] { android.R.id.text1, android.R.id.text2 }
			);
			listView.setAdapter(adapter);
			progress.dismiss();
		}
	}
}