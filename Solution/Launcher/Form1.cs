using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Threading;

namespace Launcher
{
	public partial class Form1 : Form
	{
		private string myDocumentFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
		private string myConfigPath = "Data\\Setting\\SET_config.bin";
		private string myExePath = "SpareParty_Release.exe";
		private string myLogo = "bin\\Data\\Resource\\Texture\\Logo\\T_launcher_logo.png";
		private string myGameName = "SpareParty";

		private Process myGame = new Process();

		private bool myUseCheat = true;

		enum eResolutions
		{
			R1280x720,
			R1280x1024,
			R1600x900,
			R1920x1080,
			RAuto
		}

		enum eMSAA
		{
			x1,
			x2,
			x4
		}

		public Form1()
		{

			InitializeComponent();

			string releaseConfig = myDocumentFolder + "\\Distortion Games\\" + myGameName + "\\" + myConfigPath;
			myConfigPath = releaseConfig;
			string subString = myDocumentFolder + "\\Distortion Games";
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}
			subString += "\\" + myGameName;
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}
			subString += "\\Data";
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}
			subString += "\\Setting";
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}

			//pictureBox1.Image = Image.FromFile(myLogo);
			pictureBox1.ImageLocation = Directory.GetCurrentDirectory() + "\\" + myLogo;
			
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			myResolutionList.Items.Add("1280 x 720");
			myResolutionList.Items.Add("1280 x 1024");
			myResolutionList.Items.Add("1600 x 900");
			myResolutionList.Items.Add("1920 x 1080");
			myResolutionList.Items.Add("Automatic");
			myResolutionList.SelectedIndex = 4;

			myQualityList.Items.Add("Low");
			myQualityList.Items.Add("Medium");
			myQualityList.Items.Add("High");
			myQualityList.Items.Add("Ultra");
			myQualityList.SelectedIndex = 1;

			myQualityList.DrawMode = DrawMode.OwnerDrawFixed;
			myQualityList.DrawItem += myQualityList_DrawItem;
			myQualityList.DropDownClosed += myQualityList_DropDownClosed;

			if (File.Exists(myConfigPath))
			{
				using (BinaryReader reader = new BinaryReader(File.Open(myConfigPath, FileMode.Open)))
				{
					ReadResolutionFromFile(reader);
					ReadQualityFromFile(reader);
				}
			}
		}

		void myQualityList_DropDownClosed(object sender, EventArgs e)
		{
			toolTip1.Hide(myQualityList);
		}

		void myQualityList_DrawItem(object sender, DrawItemEventArgs e)
		{
			if (e.Index < 0) { return; } // added this line thanks to Andrew's comment
			string text = "0";
			switch(e.Index)
			{
				case 0:
					text = "High Performance";
					break;
				case 1:
					text = "High Quality Lighting";
					break;
				case 2:
					text = "High Quality Lighting\nHigh Detail Models";
					break;
				case 3:
					text = "High Quality Lighting\nHigh Detail Models\nDynamic Shadows";

					break;
			}

			e.DrawBackground();
			using (SolidBrush br = new SolidBrush(e.ForeColor))
			{ e.Graphics.DrawString(myQualityList.GetItemText(myQualityList.Items[e.Index]), e.Font, br, e.Bounds); }
			if ((e.State & DrawItemState.Selected) == DrawItemState.Selected)
			{ toolTip1.Show(text, myQualityList, e.Bounds.Right, e.Bounds.Bottom); }
			e.DrawFocusRectangle();
		}

		public bool IsProcessOpen(string name)
		{
			foreach (Process runningProcess in Process.GetProcesses())
			{
				if (runningProcess.ProcessName.Contains(name) == true)
				{
					return true;
				}
			}
			return false;
		}

		private void SaveSettingsToFile()
		{
			using (BinaryWriter writer = new BinaryWriter(File.Open(myConfigPath, FileMode.Create)))
			{
				WriteResolutionToFile(writer);
				WriteQualityToFile(writer);
				WriteCheatToFile(writer);
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
			SaveSettingsToFile();

			ProcessStartInfo processInfo = new ProcessStartInfo();
			processInfo.WorkingDirectory = "bin\\";
			processInfo.FileName = myExePath;

			if (File.Exists("bin\\" + myExePath) == true)
			{
				if (IsProcessOpen("Application_Release") == true)
				{
					MessageBox.Show("A instance of the game " + myGameName + " is already running.");
				}
				else
				{
					myGame.StartInfo.FileName = myExePath;
					myGame.StartInfo.WorkingDirectory = "bin\\";
					myGame.Start();
					myGame.WaitForInputIdle();

					Application.Exit();
				}
			}
			else
			{
				MessageBox.Show("Could not find " + myExePath + ".");
			}
		}

		private void WriteCheatToFile(BinaryWriter writer)
		{
			if (myUseCheat == true)
			{
				writer.Write((Int32)1);
			}
			else
			{
				writer.Write((Int32)0);
			}
		}

		private void WriteQualityToFile(BinaryWriter writer)
		{
			Int32 quality = myQualityList.SelectedIndex;
			writer.Write(quality);
		}

		void WriteResolutionToFile(BinaryWriter aWriter)
		{
			Int32 width = 800;
			Int32 height = 600;

			Screen scr = Screen.PrimaryScreen;
			switch (myResolutionList.SelectedIndex)
			{
				case (int)eResolutions.R1280x720:
					width = 1280;
					height = 720;
					break;
				case (int)eResolutions.R1280x1024:
					width = 1280;
					height = 1024;
					break;
				case (int)eResolutions.R1600x900:
					width = 1600;
					height = 900;
					break;
				case (int)eResolutions.R1920x1080:
					width = 1920;
					height = 1080;
					break;
				default:
					width = scr.Bounds.Width;
					height = scr.Bounds.Height;

					if(width > 1920)
					{
						float ratio = (float)width / (float)height;
						width = 1920;
						height = (int)((float)width / ratio);
					}

					break;
			}

			aWriter.Write(width);
			aWriter.Write(height);
		}

		void ReadResolutionFromFile(BinaryReader aReader)
		{
			Int32 width = aReader.ReadInt32();
			Int32 height = aReader.ReadInt32();
			
			myResolutionList.SelectedIndex = 4;
		}

		void ReadQualityFromFile(BinaryReader aReader)
		{
			Int32 quality = aReader.ReadInt32();
			myQualityList.SelectedIndex = quality;
		}

		private void myQualityList_MouseHover(object sender, EventArgs e)
		{
			
		}

		private void myQualityList_MouseLeave(object sender, EventArgs e)
		{
			toolTip1.Hide(this);
		}

		private void myQualityList_SelectedIndexChanged(object sender, EventArgs e)
		{
			Point point = myQualityList.Location;
			point.X += myQualityList.Width;
			toolTip1.Show(myQualityList.SelectedIndex.ToString(), this, point);
		}
	}
}
