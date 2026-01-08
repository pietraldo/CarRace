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
			numTimeReturn.Value = configuration.TimeToReturnToTrack;
			numTimeCheck.Value = configuration.CheckPointTimeBonus;
			chBoxFog.Checked = configuration.FogEffect;
			numMinFog.Value = (int)configuration.FogMinDistance;
			numMaxFog.Value = (int)configuration.FogMaxDistance;
			chBoxDeveloperMode.Checked = configuration.ProductionMode;
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
			configuration.ProductionMode = chBoxDeveloperMode.Checked;
			this.Close();
		}
	}
}
