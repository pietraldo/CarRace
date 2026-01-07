using CarRaceLauncher.Properties;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Diagnostics;
using System.IO;


namespace CarRaceLauncher
{
	public partial class Form1 : Form
	{
		public Configuration configuration;
		public string configPathSource = "./program_files/assets/settings/default_settings.json";
		public string configPathDist = "./program_files/assets/settings/settings.json";
		public string exeFile = "./program_files/bin/carrace.exe";
		public Form1()
		{
			configuration = new Configuration(configPathSource);

			InitializeComponent();
			InitialConfigurationOfWinForm();
		}

		private void InitialConfigurationOfWinForm()
		{
			if (configuration.NumberOfPlayers == 1)
			{
				button1_Click(null, null);
			}
			else
			{
				button2_Click(null, null);
			}
			if (configuration.IsNightMode)
			{
				configuration.IsNightMode = !configuration.IsNightMode;
				pictureBox2_Click(null, null);
			}
			chBIntro.Checked = configuration.PlayIntro;
		}

		private void pictureBox1_Click(object sender, EventArgs e)
		{
			Settings formSettings = new Settings(configuration);
			formSettings.ShowDialog();
		}

		private void pictureBox2_Click(object sender, EventArgs e)
		{
			configuration.IsNightMode = !configuration.IsNightMode;
			if (configuration.IsNightMode)
				pictureBox2.Image = Resources.night;
			else
				pictureBox2.Image = Resources.day;
		}

		private void button1_Click(object sender, EventArgs e)
		{
			configuration.NumberOfPlayers = 1;
			buttonStyleSelected(button1, panel2);
			buttonStyleUnselected(button2, panel3);
		}

		private void button2_Click(object sender, EventArgs e)
		{
			configuration.NumberOfPlayers = 2;
			buttonStyleSelected(button2, panel3);
			buttonStyleUnselected(button1, panel2);
		}

		private void buttonStyleSelected(Button button, Panel panel)
		{
			panel.BackColor = Color.DarkBlue;
		}
		private void buttonStyleUnselected(Button button, Panel panel)
		{
			panel.BackColor = Color.Transparent;
		}
		public void SavePartialConfig(object partialConfig)
		{
			// 1. Read current settings
			JObject currentSettings = File.Exists(configPathSource)
				? JObject.Parse(File.ReadAllText(configPathSource))
				: new JObject();

			JsonSerializer jsonSerialzer = new JsonSerializer() { Formatting = Formatting.Indented, NullValueHandling = NullValueHandling.Ignore };

			// 2. Convert partialConfig to JObject
			JObject newSettings = JObject.FromObject(partialConfig, jsonSerialzer);

			// 3. Merge (overwrite existing fields, keep others)
			currentSettings.Merge(newSettings, new JsonMergeSettings
			{
				MergeArrayHandling = MergeArrayHandling.Replace,
				MergeNullValueHandling = MergeNullValueHandling.Ignore
			});

			// 4. Save back to file
			File.WriteAllText(configPathDist, currentSettings.ToString(Formatting.Indented));
		}

		private void button3_Click(object sender, EventArgs e)
		{
			SavePartialConfig(configuration);

			string baseDir = AppContext.BaseDirectory;
			string exePath = Path.Combine(baseDir, "program_files", "bin", "carrace.exe");
			string workingDir = Path.Combine(baseDir, "program_files");  // Working directory should be program_files/ not bin/

			if (!File.Exists(exePath))
			{
				MessageBox.Show($"Nie znaleziono pliku:\n{exePath}");
				return;
			}

			var psi = new ProcessStartInfo
			{
				FileName = exePath,
				WorkingDirectory = workingDir,
				UseShellExecute = true
			};

			Process.Start(psi);
		}

		private void chBIntro_CheckedChanged(object sender, EventArgs e)
		{
			configuration.PlayIntro = chBIntro.Checked;
		}
	}
}
