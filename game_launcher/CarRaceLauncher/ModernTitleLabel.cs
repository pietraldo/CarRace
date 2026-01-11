using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;

namespace CarRaceLauncher
{
    public class ModernTitleLabel : Control
    {
        public ModernTitleLabel()
        {
            this.SetStyle(ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint | ControlStyles.SupportsTransparentBackColor, true);
            this.DoubleBuffered = true;
            this.BackColor = Color.Transparent;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            e.Graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;

            if (string.IsNullOrEmpty(this.Text)) return;

            RectangleF rect = this.ClientRectangle;
            // Pad to ensure glow/shadow doesn't clip
            rect.Inflate(-10, -10);

            using (GraphicsPath path = new GraphicsPath())
            {
                float emSize = this.Font.Size;
                if (this.Font.Unit == GraphicsUnit.Point)
                    emSize = e.Graphics.DpiY * this.Font.SizeInPoints / 72;

                StringFormat sf = new StringFormat();
                sf.Alignment = StringAlignment.Center;
                sf.LineAlignment = StringAlignment.Center;

                path.AddString(this.Text, this.Font.FontFamily, (int)this.Font.Style, emSize, rect, sf);

                // 1. Glow (DeepSkyBlue glow - always active)
                Color glowColor = Color.DeepSkyBlue;

                // Draw multiple layers for soft glow
                for (int i = 12; i > 0; i -= 2)
                {
                    using (Pen glowPen = new Pen(Color.FromArgb(30, glowColor), i))
                    {
                        glowPen.LineJoin = LineJoin.Round;
                        e.Graphics.DrawPath(glowPen, path);
                    }
                }

                // 2. Drop Shadow (Gray/Black, 2-4px offset)
                using (GraphicsPath shadowPath = (GraphicsPath)path.Clone())
                using (Matrix mat = new Matrix())
                {
                    mat.Translate(4, 4);
                    shadowPath.Transform(mat);
                    using (Brush shadowBrush = new SolidBrush(Color.FromArgb(100, 0, 0, 0)))
                    {
                        e.Graphics.FillPath(shadowBrush, shadowPath);
                    }
                }

                // 3. Main Text (White)
                using (Brush textBrush = new SolidBrush(Color.White))
                {
                    e.Graphics.FillPath(textBrush, path);
                }
            }
        }
    }
}
