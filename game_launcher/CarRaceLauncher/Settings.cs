using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CarRaceLauncher
{
	public partial class Settings : Form
	{
		Configuration configuration;
		public Settings(Configuration configuration)
		{
			InitializeComponent();
			this.configuration = configuration;
			InitialConfigurationOfWinForm();
		}

		private void InitialConfigurationOfWinForm()
		{
			chBoxSurfaces.Checked = configuration.UseDifferentSurfaces;
			chBoxSound.Checked = configuration.Sound;
			chBoxAutoReturn.Checked = configuration.AutoReturnToTrack;
			numTimeReturn.Value = Math.Max(numTimeReturn.Minimum, Math.Min(numTimeReturn.Maximum, configuration.TimeToReturnToTrack));
			numTimeCheck.Value = Math.Max(numTimeCheck.Minimum, Math.Min(numTimeCheck.Maximum, configuration.CheckPointTimeBonus));
			chBoxFog.Checked = configuration.FogEffect;
			numMinFog.Value = Math.Max(numMinFog.Minimum, Math.Min(numMinFog.Maximum, (decimal)configuration.FogMinDistance));
			numMaxFog.Value = Math.Max(numMaxFog.Minimum, Math.Min(numMaxFog.Maximum, (decimal)configuration.FogMaxDistance));
			chBoxDeveloperMode.Checked = !configuration.ProductionMode;
			chBoxFullScreen.Checked = configuration.FullScreen;
		}

		private void button1_Click(object sender, EventArgs e)
		{
			configuration.UseDifferentSurfaces = chBoxSurfaces.Checked;
			configuration.Sound = chBoxSound.Checked;
			configuration.AutoReturnToTrack = chBoxAutoReturn.Checked;
			configuration.TimeToReturnToTrack = (int)numTimeReturn.Value;
			configuration.CheckPointTimeBonus = (int)numTimeCheck.Value;
			configuration.FogEffect = chBoxFog.Checked;
			configuration.FogMinDistance = (float)numMinFog.Value;
			configuration.FogMaxDistance = (float)numMaxFog.Value;
			configuration.ProductionMode = !chBoxDeveloperMode.Checked;
			configuration.FullScreen = chBoxFullScreen.Checked;
			this.Close();
		}

		private void chBoxFullScreen_CheckedChanged(object sender, EventArgs e)
		{

		}
	}
}
