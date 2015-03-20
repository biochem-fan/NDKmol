/*  NDKmol - Molecular Viewer on Android NDK

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
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Date;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent; 
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;

public class NDKmolActivity extends Activity {
	public static final boolean GLES1 = true;//false;
	private float startX, startY, startDistance;
	private float currentX, currentY, currentZ, currentSlabNear, currentSlabFar;
	private float currentCameraZ;
	private boolean isDragging;
	private Quaternion currentQ;
	private long lastTouchTime;
	private int touchMode = 2;
	private int prevPointerCount = 0;
	private boolean multiTouchEnabled = false;
	private String currentFilename;
	private final int IntentForURI = 1;
	private final int IntentForPreferences = 2;

	static {
		System.loadLibrary("Ndkmol");
	}

	public GLSurfaceView glSV;
	public NdkView view; 

	void initializeResource() {
		String targetPath = getDataDir() + "/2POR.pdb";
		Log.d("NDKmol", "Initializing sample data " + targetPath);
		File target = new File(targetPath);
		if (target.exists()) return;

		try {
			FileWriter out= new FileWriter(target);
			out.write(readResource(R.raw.initial));
			out.close();
		} catch (Exception e) {
			Log.d("initializeResource", "failed: " + e.toString());
		}
	}

	String readResource(int resourceId) {
		String ret = ""; 

		Resources res = this.getResources();
		InputStream st = null;
		try {
			st = res.openRawResource(resourceId);
			byte[] buffer = new byte[st.available()];
			while((st.read(buffer)) != -1) {}
			st.close();
			ret = new String(buffer);
		} catch (Exception e) {
			Log.d("ResourceOpen", e.toString());
		} finally{
		}
		return ret;
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if (Build.VERSION.SDK_INT >= 6) {
			multiTouchEnabled = true;
		}

		initializeResource();

		glSV = new GLSurfaceView(this);
		if (NDKmolActivity.GLES1) {
		    glSV.setEGLContextClientVersion(1);			
		} else {
			glSV.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
		    glSV.setEGLContextClientVersion(2);
		}
		
		view = new NdkView();
		glSV.setRenderer(view);
		applyPreferences();

		glSV.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		setContentView(glSV);

		String action = getIntent().getAction();
		if (action.equals(Intent.ACTION_VIEW)) {
			String uri = getIntent().getDataString(); 
			String fileName = uri.substring(uri.lastIndexOf('/') + 1);
			Log.d("OnCreate", "From " + uri + ", fileName: " + fileName);
			String scheme = uri.substring(0, 7);
			if (scheme.equals("file://")) {
				openFile(uri.substring(7));
			} else if (scheme.equals("http://")) {
				new Downloader(this, uri, getDataDir() + "/" + fileName);
			}
		} else {
			openFile(getDataDir() + "/2POR.pdb");
		}
	}


	public void open() {
		Intent i = new Intent(getApplicationContext(), FileBrowser.class);
		i.setData(Uri.parse("file://" + getDataDir()));
		startActivityForResult(i, IntentForURI);
	}


	public void showHeader() {
		if (currentFilename == null) return;

		FileInputStream input;
		try {
			input = new FileInputStream(currentFilename);
			InputStreamReader reader = new InputStreamReader(input);
			int headerLength = 50000;
			char buffer[] = new char[headerLength];
			reader.read(buffer, 0, headerLength);
			String header = new String(buffer);
			reader.close();
			alert("First 50KB of the file: \n\n" + header);
		} catch (Exception e) {
		}
	}

	String getDataDir() {
		String dataDir = this.getFilesDir().getAbsolutePath();
		if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
			// Although document says MEDIA_MOUNTED means a WRITABLE media exists,
			//   readonly SDcards still result in MEDIA_MOUNTED.
			File writeTest = new File(Environment.getExternalStorageDirectory().getAbsoluteFile() + "/.writeTest");
			if (writeTest.exists()) writeTest.delete();
			if (writeTest.mkdir()) {
				writeTest.delete();
				File sdfolder = new File(Environment.getExternalStorageDirectory().getAbsoluteFile() + "/PDB");
				sdfolder.mkdir();
				dataDir = sdfolder.getAbsolutePath();
			}				
		}
		Log.d("NDKmol", "Data dir is " + dataDir);
		return dataDir;
	}

	public void alert(String msg) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(msg)
		.setCancelable(false)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
			} 
		});
		AlertDialog alert = builder.create();
		alert.show();
	}

	public void readURI(String uri) {
		String scheme = uri.substring(0, 7);
		if (scheme.equals("file://")) {
			openFile(uri.substring(7));
		} else if (scheme.equals("http://")) {
			String fileName = uri.substring(uri.lastIndexOf('/') + 1);
			if (uri.contains("record_type=3d")) {
				int start = "http://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/cid/".length();
                int index = uri.indexOf("/record/SDF/?record_type=3d");
				if (index < 0) return;

				fileName = uri.substring(start, index) + ".sdf";
			}
			new Downloader(this, uri, getDataDir() + "/" + fileName);
		}
	}

	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == IntentForURI && resultCode == RESULT_OK) {
			readURI(data.getDataString());
		} else if (requestCode == IntentForPreferences) {
			applyPreferences();
		}
	}
	
	public void applyPreferences() {
		if (view == null) return;
		
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
		if (prefs.getBoolean(getString(R.string.enableFog), false)) {
			view.fogEnabled = true;
		} else {
			view.fogEnabled = false;
		}
	}

	void openFile(String filePath) {
		if (filePath.toUpperCase().endsWith("PDB")) {
			view.loadPDB(filePath);
			currentFilename = filePath;
		} else if (filePath.toUpperCase().endsWith("SDF") || filePath.toUpperCase().endsWith("MOL")) {
			view.loadSDF(filePath);
			currentFilename = filePath;
		} else if (filePath.toUpperCase().endsWith("CCP4") || filePath.toUpperCase().endsWith("CCP4.GZ")) {
			view.loadCCP4(filePath);
		} else {
			alert(getString(R.string.unknownFileType));
		}
		glSV.requestRender();
	}

	public void download() {
		Intent intent=new Intent(getApplicationContext(), SearcherTab.class);
		startActivityForResult(intent, IntentForURI);
	}

	public void preferences() {
		Intent intent = new Intent(this, MyPreferenceActivity.class);
		startActivityForResult(intent, IntentForPreferences);
	}

	public void about() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(getString(R.string.about))
		.setCancelable(false)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
			}
		})
		.setNeutralButton("Go to website", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				Uri uri = Uri.parse("http://webglmol.sfjp.jp/");
				Intent i = new Intent(Intent.ACTION_VIEW,uri);
				startActivity(i); 
			}
		});
		AlertDialog alert = builder.create();
		alert.show();
	}

	public boolean onCreateOptionsMenu(Menu menu){
		super.onCreateOptionsMenu(menu);
		getMenuInflater().inflate(R.menu.mainmenu, menu);

		return true;
	}

	// Consult View.hpp for constants
	public boolean onPrepareOptionsMenu(Menu menu) {
		if (view != null) {
			menu.findItem(R.id.Sidechain).setChecked(view.showSidechain);
			menu.findItem(R.id.Cell).setChecked(view.showUnitcell);
			menu.findItem(R.id.Solvent).setChecked(view.showSolvents);
			menu.findItem(R.id.doNotSmoothen).setChecked(view.doNotSmoothen);
			menu.findItem(R.id.symopHetatms).setChecked(view.symopHetatms);

			switch (view.proteinMode) {
			case 0:
				menu.findItem(R.id.ThickRibbon).setChecked(true);
				break;
			case 1:
				menu.findItem(R.id.Ribbon).setChecked(true);
				break;
			case 2:
				menu.findItem(R.id.CA_trace).setChecked(true);
				break;
			case 3:
				menu.findItem(R.id.Strand).setChecked(true);
				break;
			case 4:
				menu.findItem(R.id.Tube).setChecked(true);
				break;
			case 5:
				menu.findItem(R.id.Bonds).setChecked(true);
				break;
			case 6:
				menu.findItem(R.id.MainchainNone).setChecked(true);
				break;
			}

			switch (view.nucleicAcidMode) {
			case 0:
				menu.findItem(R.id.baseLine).setChecked(true);
				break;
			case 1:
				menu.findItem(R.id.basePolygon).setChecked(true);
				break;
			case 2:
				menu.findItem(R.id.baseNone).setChecked(true);
				break;
			}

			switch (view.hetatmMode) {
			case 0:
				menu.findItem(R.id.Sphere).setChecked(true);
				break;
			case 1:
				menu.findItem(R.id.Stick).setChecked(true);
				break;
			case 2:
				menu.findItem(R.id.Line).setChecked(true);
				break;
			case 3:
				menu.findItem(R.id.HetatmNone).setChecked(true);
				break;
			}

			switch (view.symmetryMode) {
			case 0:
				menu.findItem(R.id.Monomer).setChecked(true);
				break;
			case 1:
				menu.findItem(R.id.Biomt).setChecked(true);
				break;
			case 2:
				menu.findItem(R.id.Packing).setChecked(true);
			}

			switch (view.colorMode) {
			case 0:
				menu.findItem(R.id.Chainbow).setChecked(true);
				break;
			case 1:
				menu.findItem(R.id.Chain).setChecked(true);
				break;
			case 2:
				menu.findItem(R.id.Structure).setChecked(true);
				break;
			case 3:
				menu.findItem(R.id.Polarity).setChecked(true);
				break;
			case 4:
				menu.findItem(R.id.BFactor).setChecked(true);
				break;
			}

			switch (touchMode) {
			case 0:
				menu.findItem(R.id.move).setChecked(true);
				break;
			case 1:
				menu.findItem(R.id.zoom).setChecked(true);
				break;
			case 2:
				menu.findItem(R.id.rotate).setChecked(true);
				break;
			case 3:
				menu.findItem(R.id.slab).setChecked(true);
				break;
			}

		}

		return super.onPrepareOptionsMenu(menu);
	}

	public boolean onOptionsItemSelected(MenuItem item) {
		boolean ret = true;
		switch (item.getItemId()) {
		default:
			ret = super.onOptionsItemSelected(item);
			break;

		case R.id.showHeader:
			showHeader();
			break;

		case R.id.resetView: // TODO: Implement me!
			view.resetCamera();
			glSV.requestRender();
			break;
		case R.id.move:
			touchMode = 0;
			item.setChecked(true);
			break;
		case R.id.zoom:
			touchMode = 1;
			item.setChecked(true);
			break;
		case R.id.rotate:
			touchMode = 2;
			item.setChecked(true);
			break;
		case R.id.slab:
			touchMode = 3;
			item.setChecked(true);
			break;

		case R.id.open:
			open();
			break;
		case R.id.downloadPDB:
			download();
			break;
		case R.id.preferences:
			preferences();
			break;
		case R.id.about:
			about();
			break;

		case R.id.ThickRibbon:
			view.proteinMode = 0;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Ribbon:
			view.proteinMode = 1;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.CA_trace:
			view.proteinMode = 2;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Strand:
			view.proteinMode = 3;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Tube:
			view.proteinMode = 4;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Bonds:
			view.proteinMode = 5;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.MainchainNone:
			view.proteinMode = 6;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;

		case R.id.baseLine:
			view.nucleicAcidMode = 0;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.basePolygon:
			view.nucleicAcidMode = 1;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.baseNone:
			view.nucleicAcidMode = 2;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;			

		case R.id.Monomer:
			view.symmetryMode = 0;
			item.setChecked(true);
			view.prepareScene();
			view.resetCamera();
			glSV.requestRender();
			break;
		case R.id.Biomt:
			view.symmetryMode = 1;
			item.setChecked(true);
			view.prepareScene();
			view.resetCamera();
			glSV.requestRender();
			break;
		case R.id.Packing:
			view.symmetryMode = 2; 
			item.setChecked(true);
			view.prepareScene();
			view.resetCamera();
			glSV.requestRender();
			break;

		case R.id.Chainbow:
			view.colorMode = 0;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Chain:
			view.colorMode = 1;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Structure:
			view.colorMode = 2;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Polarity:
			view.colorMode = 3;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.BFactor:
			view.colorMode = 4;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;

		case R.id.Sphere:
			view.hetatmMode = 0;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Stick:
			view.hetatmMode = 1;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.Line:
			view.hetatmMode = 2;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;
		case R.id.HetatmNone:
			view.hetatmMode = 3;
			item.setChecked(true);
			view.prepareScene();
			glSV.requestRender();
			break;

		case R.id.Sidechain:
			item.setChecked(!item.isChecked());
			view.showSidechain = !view.showSidechain;
			view.prepareScene();
			glSV.requestRender();
			break;

		case R.id.Cell:
			item.setChecked(!item.isChecked());
			view.showUnitcell = !view.showUnitcell;
			view.prepareScene();
			glSV.requestRender();
			break;

		case R.id.Solvent:
			item.setChecked(!item.isChecked());
			view.showSolvents = !view.showSolvents;
			view.prepareScene();
			glSV.requestRender();
			break;
			
		case R.id.doNotSmoothen:
			item.setChecked(!item.isChecked());
			view.doNotSmoothen = !view.doNotSmoothen;
			view.prepareScene();
			glSV.requestRender();
			break;
			
		case R.id.symopHetatms:
			item.setChecked(!item.isChecked());
			view.symopHetatms = !view.symopHetatms;
			view.prepareScene();
			glSV.requestRender();
			break;
		}
		return ret;
	}

	public boolean onTouchEvent(MotionEvent e) {
		float x = e.getX();
		float y = e.getY();
		int pointerCount = 1;
		float distance = -1; 
		if (multiTouchEnabled) {
			pointerCount = MultitouchWrapper.getPointerCount(e);
		}
		if (pointerCount > 1) {
			float x1 = MultitouchWrapper.getX(e, 0);
			float x2 = MultitouchWrapper.getX(e, 1);
			float y1 = MultitouchWrapper.getY(e, 0);
			float y2 = MultitouchWrapper.getY(e, 1);
			x = (x1 + x2) / 2; y = (y1 + y2) / 2;
			distance = (float)Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
			if (prevPointerCount <= 1) {
				startDistance = distance;
			}
			Log.d("NDKmol", "multitouched dist = " + distance);
		} else if (prevPointerCount > 1) { // 2 to 1
			startX = x;
			startY = y;
		}

		switch (e.getAction()) {
		case MotionEvent.ACTION_DOWN: 
			long currentTime = new Date().getTime();
//			Log.d("event", "down delta = " + (currentTime - lastTouchTime));
			if (currentTime - lastTouchTime < 200 && pointerCount == 1) { // double tap
				// TODO: This behavior must be refined! Now it is not very intuitive.
				Vector3 vec = new Vector3((float)x / view.width - 0.5f, (float)-y / view.height + 0.5f, 0.0f);
				Vector3 translation = view.rotationQ.rotateVector(vec);
				Log.d("event", "x = " + translation.x + "y = " + translation.y + "z = " + translation.z);
				view.objX = -translation.x * 100;
				view.objY = -translation.y *100;
				view.objZ = -translation.z * 100;
				view.nativeUpdateMap(false); // TODO: FIXME: This is not working! Should use SetScene.
				glSV.requestRender(); 
			}
			lastTouchTime = currentTime;
			isDragging = true;
			view.isMoving = true;
			startX = x;
			startY = y;

			currentX = view.objX;
			currentY = view.objY;
			currentZ = view.objZ;
			currentSlabNear = view.slabNear;
			currentSlabFar = view.slabFar;
			currentCameraZ = view.cameraZ;
			currentQ = view.rotationQ;
			startDistance = -1;
			break;
		case MotionEvent.ACTION_UP:
			//			Log.d("event", "up");
			view.isMoving = false;
			isDragging = false;
			glSV.requestRender();
			break; 
		case MotionEvent.ACTION_MOVE:
			if (isDragging) {
				if (pointerCount > 1) { 
					if (startDistance > 150) { // FIXME: This is TOO small for high-density displays
						if (distance > 100) {
							view.cameraZ = currentCameraZ * startDistance / distance;
							//Log.d("NDKmol", "distance = " + distance + " start distance = " + startDistance + " CameraZ = " + view.cameraZ);
						}
					} else {
						float scaleFactor = 0.13f;
						if (view.cameraZ > -150) scaleFactor = 0.035f; // FIXME: improve
						if (view.cameraZ > -50) scaleFactor = 0.02f;
						Vector3 vec = new Vector3(- (startX - x) * scaleFactor, (startY - y) * scaleFactor, 0);
						Vector3 translation = view.rotationQ.rotateVector(vec);
						view.objX = currentX + translation.x;
						view.objY = currentY + translation.y;
						view.objZ = currentZ + translation.z;
						view.nativeUpdateMap(false);
					}
					glSV.requestRender();
				} else {
					if (touchMode == 0) { // translation
						Log.d("NDKmol", "cameraZ: " + view.cameraZ);
						float scaleFactor = 0.13f;
						if (view.cameraZ > -150) scaleFactor = 0.035f; // FIXME: improve
						if (view.cameraZ > -50) scaleFactor = 0.02f;
						Vector3 vec = new Vector3(- (startX - x) * scaleFactor, (startY - y) * scaleFactor, 0);
						Vector3 translation = view.rotationQ.rotateVector(vec);
						view.objX = currentX + translation.x;
						view.objY = currentY + translation.y;
						view.objZ = currentZ + translation.z;
						view.nativeUpdateMap(false);	
					} else if (touchMode == 1) { // zoom
						view.cameraZ = currentCameraZ + (startY - y) * 0.5f;
					} else	if (touchMode == 2) { // rotate
						float dx = (x - startX) / (float)view.width, dy = (y - startY) / (float)view.height;
						float r = (float)Math.sqrt(dx * dx + dy * dy);
						if (r == 0) return true;

						float rs = (float)Math.sin(r * Math.PI) / r;
						Quaternion dq = new Quaternion(rs * dy, rs * dx, 0,  (float)Math.cos(r * Math.PI)); 
						view.rotationQ = Quaternion.multiply(dq, currentQ);
					} else if (touchMode == 3) { // slab
						float slabLimit = view.maxD / 1.85f;
						if (view.symmetryMode != 0) slabLimit *= 2; // FIXME: improve! not working well.
						view.slabFar = currentSlabFar + (startY - y) / (float)view.height * 50;
						if (view.slabFar < 0.1f) view.slabFar = 0.1f; 
						if (view.slabFar > slabLimit) view.slabFar = slabLimit;
						view.slabNear = currentSlabNear + (startX - x) / (float)view.width * 50;
						if (view.slabNear > -0.1f) view.slabNear = -0.1f;
						if (view.slabNear < -slabLimit) view.slabNear = -slabLimit;
						Log.d("ESmol", "maxD = " + view.maxD + " slabNear = " + view.slabNear + " slabFar = " + view.slabFar);
						float slabMid = (currentSlabFar + currentSlabNear) / 2;
						float slabWidth = slabMid - currentSlabNear;
						slabWidth += (startY - y) / (float)view.height * 50;
						if (slabWidth < 0.2f) slabWidth = 0.2f;
						if (slabWidth > slabLimit) slabWidth = slabLimit;
						view.slabNear = slabMid - slabWidth;
						view.slabFar = slabMid + slabWidth;
					}
					glSV.requestRender();
				}
			}
			break;
		}

		prevPointerCount = pointerCount;
		return true;
	}
}