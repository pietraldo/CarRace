using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Windows.Forms;

namespace CarRaceLauncher
{
	public class Configuration
	{
		[JsonProperty("CAR_COUNT")]
		public int NumberOfPlayers { get; set; } = 1;

		[JsonProperty("night")]
		public bool IsNightMode { get; set; } = false;

		[JsonProperty("playIntroAnimation")]
		public bool PlayIntro { get; set; } = true;

		[JsonProperty("useDifferentMaterialsForTerrain")]
		public bool UseDifferentSurfaces { get; set; } = false;

		[JsonProperty("fog")]
		public bool FogEffect { get; set; } = false;

		[JsonProperty("fogMinDist")]
		public float FogMinDistance { get; set; } = 50;

		[JsonProperty("fogMaxDist")]
		public float FogMaxDistance { get; set; } = 200;

		[JsonProperty("productionMode")]
		public bool ProductionMode { get; set; } = true;

		[JsonProperty("fullscreen")]
		public bool FullScreen { get; set; } = false;

		[JsonProperty("sound")]
		public bool Sound { get; set; } = false;

		[JsonProperty("autoReturningToTrack")]
		public bool AutoReturnToTrack { get; set; } = true;

		[JsonProperty("timeOutsideTrackToReset")]
		public int TimeToReturnToTrack { get; set; } = 5000;

		[JsonProperty("checkpointInterval")]
		public int CheckPointTimeBonus { get; set; } = 10000;


		public Configuration() { }

		public Configuration(string filePath)
		{
			if (!File.Exists(filePath))
			{
				MessageBox.Show($"Configuration file not found:\n{filePath}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return; // leave defaults
			}

			string json = File.ReadAllText(filePath);

			try
			{
				JObject jsonObj = JObject.Parse(json);
				JsonConvert.PopulateObject(jsonObj.ToString(), this);
			}
			catch (Exception ex)
			{
				MessageBox.Show($"Failed to read configuration:\n{ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
