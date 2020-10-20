package com.showstopperled.defiance;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.Random;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.AdapterView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import com.showstopperled.defiance.ColorPickerDialog.OnColorChangedListener;

	public class MainActivity extends Activity implements OnColorChangedListener
		{
		public static final String PREFS_NAME = "DefiancePrefsFile";
		static String Packet = null;
		private BluetoothAdapter mBluetoothAdapter = null;
		private BluetoothSocket btSocket = null;
		private static OutputStream outStream = null;
		
		SeekBar seekbarUnicorns, seekbarChan, seekbarRed, seekbarGreen, seekbarBlue, seekbarBand, seekbarSens;
		TextView valueUnicorns, valueChan, valueRed, valueGreen, valueBlue, valueBand, valueSens, valueSave;
		static int Channel = 0;
		static int Rset = 255;
		static int Gset = 0;
		static int Bset = 0;
		static int Mode = 0;
		static int Band = 0;
		static int Sens = 24;
		static int Save = 0;
		static int Unicorns = 0;
	
	String audB = "63Hz";
	
	private static final UUID MY_UUID = UUID
			.fromString("00001101-0000-1000-8000-00805F9B34FB");

	// hardcoded MAC address
	private static String address = null;
	private BufferedReader buf;

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
        
        SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
        
        valueChan = (TextView) findViewById(R.id.textViewChan);
		seekbarChan = (SeekBar) findViewById(R.id.seekBarChan);
		valueRed = (TextView) findViewById(R.id.textViewRed);
		seekbarRed = (SeekBar) findViewById(R.id.seekBarRed);
		valueGreen = (TextView) findViewById(R.id.textViewGreen);
		seekbarGreen = (SeekBar) findViewById(R.id.seekBarGreen);
		valueBlue = (TextView) findViewById(R.id.textViewBlue);
		seekbarBlue = (SeekBar) findViewById(R.id.seekBarBlue);
		valueBand = (TextView) findViewById(R.id.textViewBand);
		seekbarBand = (SeekBar) findViewById(R.id.seekBarBand);
		valueSens = (TextView) findViewById(R.id.textViewSens);
		seekbarSens = (SeekBar) findViewById(R.id.seekBarSens);
		valueUnicorns = (TextView) findViewById(R.id.textViewUnicorns);
		seekbarUnicorns = (SeekBar) findViewById(R.id.seekBarUnicorns);
		
		Spinner s = (Spinner) findViewById(R.id.spinnerMode);
		s.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener(){
		 
		public void onItemSelected(AdapterView<?> adapter, View v, int i, long lng) {
		Mode = i;
		packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
		}
		 
		public void onNothingSelected(AdapterView<?> arg0) {
		//do something else
		}
		});
		
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		if (mBluetoothAdapter == null) {
			Toast.makeText(this, "Bluetooth is not available.",
					Toast.LENGTH_LONG).show();
			finish();
			return;
		}

		if (!mBluetoothAdapter.isEnabled()) {
			Toast.makeText(this,
					"Please enable your BT and re-run this program.",
					Toast.LENGTH_LONG).show();
			finish();
			return;
		}

		int sbar_chan_position = settings.getInt("seekbar_chan_pref", 0);
		seekbarChan.setProgress(sbar_chan_position);
		valueChan.setText("Channel: " + sbar_chan_position);
		Channel = sbar_chan_position;
		
		int sbar_red_position = settings.getInt("seekbar_red_pref", 0);
		seekbarRed.setProgress(sbar_red_position);
		valueRed.setText("Red: " + sbar_red_position);
		Rset = sbar_red_position;
		
		int sbar_green_position = settings.getInt("seekbar_green_pref", 0);
		seekbarGreen.setProgress(sbar_green_position);
		valueGreen.setText("Green: " + sbar_green_position);
		Gset = sbar_green_position;
		
		int sbar_blue_position = settings.getInt("seekbar_blue_pref", 0);
		seekbarBlue.setProgress(sbar_blue_position);
		valueBlue.setText("Blue: " + sbar_blue_position);
		Bset = sbar_blue_position;
		
		int sbar_band_position = settings.getInt("seekbar_band_pref", 0);
		seekbarBand.setProgress(sbar_band_position);
		valueBand.setText("Band: " + sbar_band_position);
		Band = sbar_band_position;
		
		int sbar_sens_position = settings.getInt("seekbar_sens_pref", 0);
		seekbarSens.setProgress(sbar_sens_position);
		valueSens.setText("Sensitivity: " + sbar_sens_position);
		Sens = sbar_sens_position;
		
		int sbar_unicorns_position = settings.getInt("seekbar_unicorns_pref", 0);
		seekbarUnicorns.setProgress(sbar_unicorns_position);
		valueUnicorns.setText("Unicorns!: " + sbar_unicorns_position);
		Unicorns = sbar_unicorns_position;
		
		seekbarChan.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				valueChan.setText("Channel: " + (progress + 1));
				Channel = progress;
				packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
				

			}
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
		});
			
seekbarRed.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				valueRed.setText("Red: " + progress);

				Rset = progress;
				packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);

			}
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
		});
			
seekbarGreen.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				valueGreen.setText("Green: " + progress);

				Gset = progress;
				packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);


			}
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
		});
			
seekbarBlue.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				valueBlue.setText("Blue: " + progress);

				Bset = progress;
				packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);

			}
			
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
		});
			
seekbarBand.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
					switch(progress)
					{
					case 0:
					audB = "63Hz";
					break;
					case 1:
					audB = "160Hz";
					break;
					case 2:
					audB = "400Hz";
					break;
					case 3:
					audB = "1kHz";
					break;
					case 4:
					audB = "2.5kHz";
					break;
					case 5:
					audB = "6.25kHz";
					break;
					case 6:
					audB = "16kHz";
					break;
					}
				valueBand.setText("Band: " + audB);

				Band = progress;
				packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);

			}
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
		});
			
seekbarSens.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				valueSens.setText("Sensitivity: " + (progress + 1));

				Sens = progress;
				packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);

			}
			
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
		}); 

seekbarUnicorns.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {
		valueUnicorns.setText("Unicorns!: " + progress);

		Unicorns = progress;
		packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);

	}
	
	public void onStartTrackingTouch(SeekBar seekBar) {
		// TODO Auto-generated method stub
	}

	public void onStopTrackingTouch(SeekBar seekBar) {
		// TODO Auto-generated method stub
	}
}); 
        
    }

   

    public void colorChanged(int color) {

		int sbar_red_position = Color.red(color);
		int sbar_green_position = Color.green(color);
		int sbar_blue_position = Color.blue(color);

		seekbarRed.setProgress(sbar_red_position);
		valueRed.setText("Red: " + sbar_red_position);
		Rset = sbar_red_position;

		seekbarGreen.setProgress(sbar_green_position);
		valueGreen.setText("Green: " + sbar_green_position);
		Gset = sbar_green_position;

		seekbarBlue.setProgress(sbar_blue_position);
		valueBlue.setText("Blue: " + sbar_blue_position);
		Bset = sbar_blue_position;
		packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	}
	
    @Override
    public void onStart() {
        super.onStart();
    }



    @Override
	public void onResume() {
		super.onResume();
		try{
			   File f = new File(Environment.getExternalStorageDirectory()+"/Defiance.txt");
			   FileInputStream fileIS = new FileInputStream(f);
			   buf = new BufferedReader(new InputStreamReader(fileIS));
			   String readString = new String(); 
			   //just reading each line and pass it on the debugger
			   while((readString = buf.readLine())!= null){
			      address = readString;
			   }
			} catch (FileNotFoundException e) {
			   e.printStackTrace();
			} catch (IOException e){
			   e.printStackTrace();
			}

		// When this returns, it will 'know' about the server,
		// via it's MAC address.
		BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);

		try {
			btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
		} catch (IOException e) {
		}

		mBluetoothAdapter.cancelDiscovery();

		try {
			btSocket.connect();
		} catch (IOException e) {
			try {
				btSocket.close();
			} catch (IOException e2) {
			}
		}

		// Create a data stream so we can talk to server.
		try {
			outStream = btSocket.getOutputStream();
		} catch (IOException e) {
		}
	}
	
	    public void onClickSave1(View v) throws IOException {
	    	Save = 100;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickSave2(View v) throws IOException {
	    	Save = 200;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickSave3(View v) throws IOException {
	    	Save = 300;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickSave4(View v) throws IOException {
	    	Save = 400;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickSave5(View v) throws IOException {
	    	Save = 500;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickLoad1(View v) throws IOException {
	    	Save = 10;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickLoad2(View v) throws IOException {
	    	Save = 20;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickLoad3(View v) throws IOException {
	    	Save = 30;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickLoad4(View v) throws IOException {
	    	Save = 40;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    public void onClickLoad5(View v) throws IOException {
	    	Save = 50;
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	Save = 0;
	    	}
	    
	    public void onClickApply(View v) throws IOException {
	    	
	    	packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	    	}
	
	    @Override
		public void onPause() {
			super.onPause();

			SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
			SharedPreferences.Editor editor = settings.edit();
			editor.putInt("seekbar_red_pref", Rset);
			editor.putInt("seekbar_green_pref", Gset);
			editor.putInt("seekbar_blue_pref", Bset);
			editor.commit();

			if (outStream != null) {
				try {
					outStream.flush();
				} catch (IOException e) {
				}
			}

			try {
				btSocket.close();
			} catch (IOException e2) {
			}
			}
	
	@Override
	public void onStop() {
		super.onStop();
	}
	
    @Override
    public void onDestroy() {
        super.onDestroy();
        // Stop the Bluetooth chat services
        
    }
 
		
	public void onClickColorPicker(View v) {
		new ColorPickerDialog(this, this, Color.rgb(Rset, Gset, Bset)).show();
		packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	}
	
	public void onClickRandomColor(View v) {
		    Random rnd = new Random();
		    int min = 0, max = 255;
		    int randomRed = rnd.nextInt(max - min + 1) + min;
		    int randomGreen = rnd.nextInt(max - min + 1) + min;
		    int randomBlue = rnd.nextInt(max - min + 1) + min;
		    Rset = randomRed;
		    Gset = randomGreen;
		    Bset = randomBlue;
		    new ColorPickerDialog(this, this, Color.rgb(Rset, Gset, Bset)).show();
		    packet(Channel, Rset, Gset, Bset, Mode, Band, Sens, Unicorns, Save);
	}

	
	public static void packet(int Channel, int Rset, int Gset, int Bset,int Mode, int Band, int Sens, int Unicorns, int Save) {
		int Rfin;
		int Gfin;
		int Bfin;
		int Sfin;
		int Ufin;
		Rfin = Rset * 16;
		Rfin = 4095 - Rfin;
		Gfin = Gset * 16;
		Gfin = 4095 - Gfin;
		Bfin = Bset * 16;
		Bfin = 4095 - Bfin;
		Sfin = Sens * 32;
		Ufin = Unicorns * 10;
		Ufin = 1000 - Ufin;
		Packet = "<HMG:" + Channel + ":" + Rfin + ":" + Gfin + ":" + Bfin + ":" + Mode + ":" + Band + ":" + Sfin + ":" + Ufin + ":" + Save + ">";
		byte[] msgBuffer;
        msgBuffer = Packet.getBytes();
		try {
			outStream.write(msgBuffer);
			} catch (IOException e) {
			}
		}	
	}