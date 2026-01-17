namespace CarRaceLauncher
{
	partial class Settings
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			chBoxSurfaces = new CheckBox();
			chBoxDeveloperMode = new CheckBox();
			chBoxAutoReturn = new CheckBox();
			groupBox1 = new GroupBox();
			label2 = new Label();
			numTimeCheck = new NumericUpDown();
			label1 = new Label();
			numTimeReturn = new NumericUpDown();
			groupBox2 = new GroupBox();
			label3 = new Label();
			chBoxFog = new CheckBox();
			numMaxFog = new NumericUpDown();
			label4 = new Label();
			numMinFog = new NumericUpDown();
			button1 = new Button();
			tableLayoutPanel1 = new TableLayoutPanel();
			chBoxSound = new CheckBox();
			chBoxFullScreen = new CheckBox();
			groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)numTimeCheck).BeginInit();
			((System.ComponentModel.ISupportInitialize)numTimeReturn).BeginInit();
			groupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)numMaxFog).BeginInit();
			((System.ComponentModel.ISupportInitialize)numMinFog).BeginInit();
			tableLayoutPanel1.SuspendLayout();
			SuspendLayout();
			// 
			// chBoxSurfaces
			// 
			chBoxSurfaces.AutoSize = true;
			chBoxSurfaces.Checked = true;
			chBoxSurfaces.CheckState = CheckState.Checked;
			chBoxSurfaces.Location = new Point(19, 12);
			chBoxSurfaces.Name = "chBoxSurfaces";
			chBoxSurfaces.Size = new Size(272, 19);
			chBoxSurfaces.TabIndex = 0;
			chBoxSurfaces.Text = "Use different surfaces (ex. car on grass will slip)";
			chBoxSurfaces.UseVisualStyleBackColor = true;
			// 
			// chBoxDeveloperMode
			// 
			chBoxDeveloperMode.AutoSize = true;
			chBoxDeveloperMode.Location = new Point(12, 345);
			chBoxDeveloperMode.Name = "chBoxDeveloperMode";
			chBoxDeveloperMode.Size = new Size(116, 19);
			chBoxDeveloperMode.TabIndex = 1;
			chBoxDeveloperMode.Text = "Developer mode ";
			chBoxDeveloperMode.UseVisualStyleBackColor = true;
			// 
			// chBoxAutoReturn
			// 
			chBoxAutoReturn.AutoSize = true;
			chBoxAutoReturn.Location = new Point(7, 22);
			chBoxAutoReturn.Name = "chBoxAutoReturn";
			chBoxAutoReturn.Size = new Size(147, 19);
			chBoxAutoReturn.TabIndex = 2;
			chBoxAutoReturn.Text = "Auto returning to track";
			chBoxAutoReturn.UseVisualStyleBackColor = true;
			// 
			// groupBox1
			// 
			groupBox1.Controls.Add(label2);
			groupBox1.Controls.Add(chBoxAutoReturn);
			groupBox1.Controls.Add(numTimeCheck);
			groupBox1.Controls.Add(label1);
			groupBox1.Controls.Add(numTimeReturn);
			groupBox1.Location = new Point(12, 91);
			groupBox1.Name = "groupBox1";
			groupBox1.Size = new Size(438, 121);
			groupBox1.TabIndex = 3;
			groupBox1.TabStop = false;
			groupBox1.Text = "Returning to track";
			// 
			// label2
			// 
			label2.AutoSize = true;
			label2.Location = new Point(133, 84);
			label2.Name = "label2";
			label2.Size = new Size(269, 15);
			label2.TabIndex = 6;
			label2.Text = "How often checkpoint will be made (miliseconds)";
			// 
			// numTimeCheck
			// 
			numTimeCheck.Location = new Point(7, 82);
			numTimeCheck.Maximum = new decimal(new int[] { 100000, 0, 0, 0 });
			numTimeCheck.Minimum = new decimal(new int[] { 100, 0, 0, 0 });
			numTimeCheck.Name = "numTimeCheck";
			numTimeCheck.Size = new Size(120, 23);
			numTimeCheck.TabIndex = 5;
			numTimeCheck.Value = new decimal(new int[] { 1500, 0, 0, 0 });
			// 
			// label1
			// 
			label1.AutoSize = true;
			label1.Location = new Point(133, 49);
			label1.Name = "label1";
			label1.Size = new Size(288, 15);
			label1.TabIndex = 4;
			label1.Text = "Time after which car be moved to track (miliseconds)";
			// 
			// numTimeReturn
			// 
			numTimeReturn.Location = new Point(7, 47);
			numTimeReturn.Maximum = new decimal(new int[] { 100000, 0, 0, 0 });
			numTimeReturn.Minimum = new decimal(new int[] { 50, 0, 0, 0 });
			numTimeReturn.Name = "numTimeReturn";
			numTimeReturn.Size = new Size(120, 23);
			numTimeReturn.TabIndex = 0;
			numTimeReturn.Value = new decimal(new int[] { 2000, 0, 0, 0 });
			// 
			// groupBox2
			// 
			groupBox2.Controls.Add(label3);
			groupBox2.Controls.Add(chBoxFog);
			groupBox2.Controls.Add(numMaxFog);
			groupBox2.Controls.Add(label4);
			groupBox2.Controls.Add(numMinFog);
			groupBox2.Location = new Point(12, 218);
			groupBox2.Name = "groupBox2";
			groupBox2.Size = new Size(438, 121);
			groupBox2.TabIndex = 7;
			groupBox2.TabStop = false;
			groupBox2.Text = "Fog";
			// 
			// label3
			// 
			label3.AutoSize = true;
			label3.Location = new Point(133, 84);
			label3.Name = "label3";
			label3.Size = new Size(129, 15);
			label3.TabIndex = 6;
			label3.Text = "Maximum fog distance";
			// 
			// chBoxFog
			// 
			chBoxFog.AutoSize = true;
			chBoxFog.Location = new Point(7, 22);
			chBoxFog.Name = "chBoxFog";
			chBoxFog.Size = new Size(46, 19);
			chBoxFog.TabIndex = 2;
			chBoxFog.Text = "Fog";
			chBoxFog.UseVisualStyleBackColor = true;
			// 
			// numMaxFog
			// 
			numMaxFog.Location = new Point(7, 82);
			numMaxFog.Maximum = new decimal(new int[] { 100000, 0, 0, 0 });
			numMaxFog.Minimum = new decimal(new int[] { 10, 0, 0, 0 });
			numMaxFog.Name = "numMaxFog";
			numMaxFog.Size = new Size(120, 23);
			numMaxFog.TabIndex = 5;
			numMaxFog.Value = new decimal(new int[] { 100, 0, 0, 0 });
			// 
			// label4
			// 
			label4.AutoSize = true;
			label4.Location = new Point(133, 49);
			label4.Name = "label4";
			label4.Size = new Size(128, 15);
			label4.TabIndex = 4;
			label4.Text = "Minimum fog distance";
			// 
			// numMinFog
			// 
			numMinFog.Location = new Point(7, 47);
			numMinFog.Maximum = new decimal(new int[] { 100000, 0, 0, 0 });
			numMinFog.Name = "numMinFog";
			numMinFog.Size = new Size(120, 23);
			numMinFog.TabIndex = 0;
			numMinFog.Value = new decimal(new int[] { 50, 0, 0, 0 });
			// 
			// button1
			// 
			button1.Anchor = AnchorStyles.None;
			button1.BackColor = Color.FromArgb(0, 192, 0);
			button1.FlatStyle = FlatStyle.Flat;
			button1.Location = new Point(204, 12);
			button1.Name = "button1";
			button1.Size = new Size(84, 31);
			button1.TabIndex = 8;
			button1.Text = "OK";
			button1.UseVisualStyleBackColor = false;
			button1.Click += button1_Click;
			// 
			// tableLayoutPanel1
			// 
			tableLayoutPanel1.ColumnCount = 1;
			tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
			tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
			tableLayoutPanel1.Controls.Add(button1, 0, 0);
			tableLayoutPanel1.Dock = DockStyle.Bottom;
			tableLayoutPanel1.Location = new Point(0, 385);
			tableLayoutPanel1.Name = "tableLayoutPanel1";
			tableLayoutPanel1.RowCount = 1;
			tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Percent, 50F));
			tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Percent, 50F));
			tableLayoutPanel1.Size = new Size(493, 56);
			tableLayoutPanel1.TabIndex = 9;
			// 
			// chBoxSound
			// 
			chBoxSound.AutoSize = true;
			chBoxSound.Checked = true;
			chBoxSound.CheckState = CheckState.Checked;
			chBoxSound.Location = new Point(19, 37);
			chBoxSound.Name = "chBoxSound";
			chBoxSound.Size = new Size(97, 19);
			chBoxSound.TabIndex = 10;
			chBoxSound.Text = "Enable sound";
			chBoxSound.UseVisualStyleBackColor = true;
			// 
			// chBoxFullScreen
			// 
			chBoxFullScreen.AutoSize = true;
			chBoxFullScreen.Checked = true;
			chBoxFullScreen.CheckState = CheckState.Checked;
			chBoxFullScreen.Location = new Point(19, 62);
			chBoxFullScreen.Name = "chBoxFullScreen";
			chBoxFullScreen.Size = new Size(82, 19);
			chBoxFullScreen.TabIndex = 11;
			chBoxFullScreen.Text = "Full screen";
			chBoxFullScreen.UseVisualStyleBackColor = true;
			chBoxFullScreen.CheckedChanged += chBoxFullScreen_CheckedChanged;
			// 
			// Settings
			// 
			AutoScaleDimensions = new SizeF(7F, 15F);
			AutoScaleMode = AutoScaleMode.Font;
			ClientSize = new Size(493, 441);
			Controls.Add(chBoxFullScreen);
			Controls.Add(chBoxSound);
			Controls.Add(tableLayoutPanel1);
			Controls.Add(groupBox2);
			Controls.Add(groupBox1);
			Controls.Add(chBoxDeveloperMode);
			Controls.Add(chBoxSurfaces);
			FormBorderStyle = FormBorderStyle.SizableToolWindow;
			Name = "Settings";
			Text = "Settings";
			groupBox1.ResumeLayout(false);
			groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)numTimeCheck).EndInit();
			((System.ComponentModel.ISupportInitialize)numTimeReturn).EndInit();
			groupBox2.ResumeLayout(false);
			groupBox2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)numMaxFog).EndInit();
			((System.ComponentModel.ISupportInitialize)numMinFog).EndInit();
			tableLayoutPanel1.ResumeLayout(false);
			ResumeLayout(false);
			PerformLayout();
		}

		#endregion

		private CheckBox chBoxSurfaces;
		private CheckBox chBoxDeveloperMode;
		private CheckBox chBoxAutoReturn;
		private GroupBox groupBox1;
		private Label label1;
		private NumericUpDown numTimeReturn;
		private Label label2;
		private NumericUpDown numTimeCheck;
		private GroupBox groupBox2;
		private Label label3;
		private CheckBox chBoxFog;
		private NumericUpDown numMaxFog;
		private Label label4;
		private NumericUpDown numMinFog;
		private Button button1;
		private TableLayoutPanel tableLayoutPanel1;
		private CheckBox chBoxSound;
		private CheckBox chBoxFullScreen;
	}
}