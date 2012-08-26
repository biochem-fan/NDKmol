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
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.net.SocketAddress;
import java.net.URL;
import java.net.URLEncoder;
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
import android.widget.AdapterView.OnItemClickListener;

public class PubChemSearcher extends Activity {

	private ListView listView = null;
	private Button searchButton;
	private EditText keyword;
	private String pubchemSearchURI = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=pccompound&retmax=#MAXRESULT#&term=#KEYWORD#";
	private String pdbchemDetailURI = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?db=pccompound&id=#IDs#";
	private String pubchemDownloadURI = "http://pubchem.ncbi.nlm.nih.gov/summary/summary.cgi?disopt=3DSaveSDF&cid=";
	private List<Map<String, String>> dataList;
	private int MAXRESULT = 30, MAXSYNONYMS = 5;
	private PubChemSearcher self;
	private View detailsView;
	private Proxy proxy;

	private ArrayList<String> queryPubchemforIDs(String keyword) {
		ArrayList<String> ids = new ArrayList<String>();
		StringBuffer sb = new StringBuffer();

		try {
			keyword = URLEncoder.encode(keyword, "UTF-8");
			URL url = new URL(pubchemSearchURI.replaceFirst("#KEYWORD#", keyword).replaceFirst("#MAXRESULT#", Integer.toString(MAXRESULT)));
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
			Log.d("queryPubChem", e.toString());
		}

		int pos = 0, lindex, rindex;
		while ((lindex = sb.indexOf("<Id>", pos)) > 0) {
			lindex += 4;
			rindex = sb.indexOf("</Id>", lindex);
			if (rindex < 0) break; // Something is wrong!
			ids.add(sb.substring(lindex, rindex));
			pos = rindex + 1;
		}

		Log.d("Pubchem", ids.toString());
		return ids;
	}

	private List<Map<String,String>> queryPubchemforDetails(ArrayList<String> ids) {
		List<Map<String,String>> ret = new ArrayList<Map<String,String>>();
		String joined = "";
		StringBuffer sb = new StringBuffer();

		int lim = ids.size();
		if (lim > MAXRESULT) lim = MAXRESULT;
		for (int i = 0; i < lim; i++) joined += ids.get(i) + ","; // final ',' doesn't harm

		try {
			URL url = new URL(pdbchemDetailURI.replaceFirst("#IDs#", joined));
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
			Log.d("queryPubChem", e.toString());
		}

		String[] entries = sb.toString().split("</DocSum>");
		for (int i = 0, ilim = entries.length; i < ilim; i++) {
			String entry = entries[i];
			HashMap<String, String> records = new HashMap<String, String>();
			
			int idL = entry.indexOf("<Id>");
			int idR = entry.indexOf("</Id>");
			if (idL < 0 || idR < 0) continue;
			idL += 4;
			String id = entry.substring(idL, idR);
			String synonyms = "";
			
			int pos = entry.indexOf("<Item Name=\"SynonymList\" Type=\"List\">");
			int lindex, rindex, cnt = 0;
			while ((lindex = entry.indexOf("<Item Name=\"string\" Type=\"String\">", pos)) > 0) {
				 lindex += 34;
				 rindex = entry.indexOf("</Item>", lindex);
				 synonyms += entry.substring(lindex, rindex) + " ";
			     pos = rindex + 1;
			     int peek = entry.indexOf("</Item>", rindex + 1);
			     if (peek - rindex < 30) break;
			     if (cnt++ > MAXSYNONYMS) break;
			}
			records.put("structureId", id);
			records.put("structureTitle", synonyms);
			ret.add(records);
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
		dataList = null;
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
				// TODO: Can we recycle it by removeView?
				LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
				detailsView = inflater.inflate(R.layout.detailview, null);

				((TextView)detailsView.findViewById(R.id.textID)).setText(PDBid);
				((TextView)detailsView.findViewById(R.id.textTitle)).setText(item.get("structureTitle"));
				((TextView)detailsView.findViewById(R.id.textResolution)).setText("N/A");
				((TextView)detailsView.findViewById(R.id.textAuthors)).setText("N/A");
				((TextView)detailsView.findViewById(R.id.textReleaseDate)).setText("N/A");
				Builder b = new AlertDialog.Builder(self)
				.setIcon(android.R.drawable.ic_dialog_info)
				.setTitle("Structure details")
				.setView(detailsView)
				.setPositiveButton("Download", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						Intent i = new Intent();
						i.setData(Uri.parse(pubchemDownloadURI + PDBid));
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
			progress = new ProgressDialog(PubChemSearcher.this);

			progress.setTitle(PubChemSearcher.this.getString(R.string.searching));
			progress.setMessage(PubChemSearcher.this.getString(R.string.pleaseWait));
			progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
			progress.show();
		}

		@Override
		protected Boolean doInBackground(String... searchFor) {
			ArrayList<String> ids = queryPubchemforIDs(searchFor[0]);
			dataList = queryPubchemforDetails(ids);
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