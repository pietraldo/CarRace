namespace CarRaceLauncher
{
	partial class Form1
	{
		/// <summary>
		///  Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		///  Clean up any resources being used.
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
		///  Required method for Designer support - do not modify
		///  the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
			label1 = new ModernTitleLabel();
			panel1 = new Panel();
			pictureBox1 = new PictureBox();
			button1 = new ModernButton();
			button2 = new ModernButton();
			button3 = new ModernButton();
			pictureBox2 = new PictureBox();
			tableLayoutPanel1 = new TableLayoutPanel();
			tableLayoutPanel2 = new TableLayoutPanel();
			panel3 = new Panel();
			panel2 = new Panel();
			panel1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)pictureBox1).BeginInit();
			((System.ComponentModel.ISupportInitialize)pictureBox2).BeginInit();
			tableLayoutPanel1.SuspendLayout();
			tableLayoutPanel2.SuspendLayout();
			panel3.SuspendLayout();
			panel2.SuspendLayout();
			SuspendLayout();
			// 
			// label1
			// 
			label1.BackColor = Color.Transparent;
			label1.Font = new Font("Segoe UI", 72F, FontStyle.Bold, GraphicsUnit.Point, 238);
			label1.ForeColor = Color.White;
			label1.Location = new Point(0, 0);
			label1.Name = "label1";
			label1.Size = new Size(960, 150);
			label1.TabIndex = 0;
			label1.Text = "CAR RACE";
			// 
			// panel1
			// 
			panel1.BackColor = Color.Transparent;
			panel1.Controls.Add(pictureBox1);
			panel1.Controls.Add(label1);
			panel1.Dock = DockStyle.Top;
			panel1.Location = new Point(0, 0);
			panel1.Name = "panel1";
			panel1.Size = new Size(960, 150);
			panel1.TabIndex = 1;
			// 
			// pictureBox1
			// 
			pictureBox1.Anchor = AnchorStyles.Top | AnchorStyles.Right;
			pictureBox1.Image = Properties.Resources.setting;
			pictureBox1.Location = new Point(870, 12);
			pictureBox1.Name = "pictureBox1";
			pictureBox1.Size = new Size(75, 75);
			pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
			pictureBox1.TabIndex = 5;
			pictureBox1.TabStop = false;
			pictureBox1.Click += pictureBox1_Click;
			// 
			// button1
			// 
			button1.BackColor = Color.DarkOrange;
			button1.BorderRadius = 40;
			button1.FlatAppearance.BorderSize = 0;
			button1.FlatStyle = FlatStyle.Flat;
			button1.Font = new Font("Segoe UI Black", 20F, FontStyle.Bold, GraphicsUnit.Point, 238);
			button1.ForeColor = Color.White;
			button1.Location = new Point(0, 0);
			button1.Margin = new Padding(0);
			button1.Name = "button1";
			button1.Size = new Size(220, 70);
			button1.TabIndex = 2;
			button1.Text = "1 Player";
			button1.UseVisualStyleBackColor = false;
			button1.Click += button1_Click;
			// 
			// button2
			// 
			button2.Anchor = AnchorStyles.Left;
			button2.BackColor = Color.RoyalBlue;
			button2.BorderRadius = 40;
			button2.FlatAppearance.BorderSize = 0;
			button2.FlatStyle = FlatStyle.Flat;
			button2.Font = new Font("Segoe UI Black", 20F, FontStyle.Bold, GraphicsUnit.Point, 238);
			button2.ForeColor = Color.White;
			button2.Location = new Point(0, 0);
			button2.Margin = new Padding(0);
			button2.Name = "button2";
			button2.Size = new Size(220, 70);
			button2.TabIndex = 3;
			button2.Text = "2 Players";
			button2.UseVisualStyleBackColor = false;
			button2.Click += button2_Click;
			// 
			// button3
			// 
			button3.Anchor = AnchorStyles.None;
			button3.BackColor = Color.LimeGreen;
			button3.BorderRadius = 50;
			button3.FlatAppearance.BorderSize = 0;
			button3.FlatStyle = FlatStyle.Flat;
			button3.Font = new Font("Segoe UI Black", 32F, FontStyle.Bold, GraphicsUnit.Point, 238);
			button3.Location = new Point(330, 194);
			button3.Name = "button3";
			button3.Size = new Size(300, 100);
			button3.TabIndex = 4;
			button3.Text = "PLAY";
			button3.UseVisualStyleBackColor = false;
			button3.Click += button3_Click;
			// 
			// pictureBox2
			// 
			pictureBox2.Anchor = AnchorStyles.None;
			pictureBox2.Image = (Image)resources.GetObject("pictureBox2.Image");
			pictureBox2.Location = new Point(365, 22);
			pictureBox2.Name = "pictureBox2";
			pictureBox2.Size = new Size(229, 75);
			pictureBox2.SizeMode = PictureBoxSizeMode.StretchImage;
			pictureBox2.TabIndex = 6;
			pictureBox2.TabStop = false;
			pictureBox2.Click += pictureBox2_Click;
			// 
			// tableLayoutPanel1
			// 
			tableLayoutPanel1.BackColor = Color.Transparent;
			tableLayoutPanel1.ColumnCount = 1;
			tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
			tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
			tableLayoutPanel1.Controls.Add(button3, 0, 2);
			tableLayoutPanel1.Controls.Add(pictureBox2, 0, 0);
			tableLayoutPanel1.Dock = DockStyle.Bottom;
			tableLayoutPanel1.Location = new Point(0, 393);
			tableLayoutPanel1.Name = "tableLayoutPanel1";
			tableLayoutPanel1.RowCount = 3;
			tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Percent, 74.3801651F));
			tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Percent, 25.6198349F));
			tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Absolute, 140F));
			tableLayoutPanel1.Size = new Size(960, 282);
			tableLayoutPanel1.TabIndex = 7;
			// 
			// tableLayoutPanel2
			// 
			tableLayoutPanel2.BackColor = Color.Transparent;
			tableLayoutPanel2.ColumnCount = 2;
			tableLayoutPanel2.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
			tableLayoutPanel2.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
			tableLayoutPanel2.Controls.Add(panel3, 1, 0);
			tableLayoutPanel2.Controls.Add(panel2, 0, 0);
			tableLayoutPanel2.Dock = DockStyle.Bottom;
			tableLayoutPanel2.Location = new Point(0, 226);
			tableLayoutPanel2.Name = "tableLayoutPanel2";
			tableLayoutPanel2.RowCount = 1;
			tableLayoutPanel2.RowStyles.Add(new RowStyle(SizeType.Percent, 50F));
			tableLayoutPanel2.RowStyles.Add(new RowStyle(SizeType.Percent, 50F));
			tableLayoutPanel2.Size = new Size(960, 167);
			tableLayoutPanel2.TabIndex = 8;
			// 
			// panel3
			// 
			panel3.BackColor = Color.Transparent;
			panel3.Anchor = AnchorStyles.Left;
			panel3.Controls.Add(button2);
			panel3.Location = new Point(510, 46);
			panel3.Margin = new Padding(30, 9, 9, 9);
			panel3.Name = "panel3";
			panel3.Size = new Size(230, 80);
			panel3.TabIndex = 9;
			// 
			// panel2
			// 
			panel2.BackColor = Color.Transparent;
			panel2.Anchor = AnchorStyles.Right;
			panel2.Controls.Add(button1);
			panel2.Location = new Point(220, 46);
			panel2.Margin = new Padding(4, 4, 30, 4);
			panel2.Name = "panel2";
			panel2.Size = new Size(230, 80);
			panel2.TabIndex = 6;
			// 
			// Form1
			// 
			AutoScaleDimensions = new SizeF(7F, 15F);
			AutoScaleMode = AutoScaleMode.Font;
			BackgroundImage = Properties.Resources.ChatGPT_Image_4_sty_2026__14_15_11;
			BackgroundImageLayout = ImageLayout.Stretch;
			ClientSize = new Size(960, 675);
			Controls.Add(tableLayoutPanel2);
			Controls.Add(tableLayoutPanel1);
			Controls.Add(panel1);
			Icon = (Icon)resources.GetObject("$this.Icon");
			Name = "Form1";
			StartPosition = FormStartPosition.CenterScreen;
			Text = "Car Race Launcher";
			panel1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)pictureBox1).EndInit();
			((System.ComponentModel.ISupportInitialize)pictureBox2).EndInit();
			tableLayoutPanel1.ResumeLayout(false);
			tableLayoutPanel1.PerformLayout();
			tableLayoutPanel2.ResumeLayout(false);
			panel3.ResumeLayout(false);
			panel2.ResumeLayout(false);
			ResumeLayout(false);
		}

		#endregion

		private ModernTitleLabel label1;
		private Panel panel1;
		private ModernButton button1;
		private ModernButton button2;
		private ModernButton button3;
		private PictureBox pictureBox1;
		private PictureBox pictureBox2;
		private TableLayoutPanel tableLayoutPanel1;
		private TableLayoutPanel tableLayoutPanel2;
		private Panel panel2;
		private Panel panel3;
	}
}
