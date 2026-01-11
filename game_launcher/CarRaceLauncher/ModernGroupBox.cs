using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace CarRaceLauncher
{
    public class ModernGroupBox : GroupBox
    {
        private Color _borderColor = Color.MediumSlateBlue;
        private Color _textColor = Color.White;
        private Color _titleBackColor = Color.FromArgb(30, 30, 30); // Matches form background usually
        private int _borderWidth = 2;

        public ModernGroupBox()
        {
            this.DoubleBuffered = true;
            this.SetStyle(ControlStyles.UserPaint | ControlStyles.ResizeRedraw | ControlStyles.SupportsTransparentBackColor | ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer, true);
            this.BackColor = Color.Transparent;
            this.ForeColor = _textColor;
        }

        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public Color BorderColor
        {
            get { return _borderColor; }
            set { _borderColor = value; this.Invalidate(); }
        }

        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public Color TitleBackColor
        {
            get { return _titleBackColor; }
            set { _titleBackColor = value; this.Invalidate(); }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.ClearTypeGridFit;

            // Define the rectangle for the border
            Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);

            // Calculate text size to know where to break the border
            SizeF textSize = g.MeasureString(this.Text, this.Font);
            RectangleF textRect = new RectangleF(10, 0, textSize.Width, textSize.Height);

            // Adjust border rect to start below half the text height
            int topOffset = (int)(textSize.Height / 2);
            Rectangle borderRect = new Rectangle(1, topOffset, rect.Width - 2, rect.Height - topOffset - 1);

            // Draw Border
            using (GraphicsPath path = GetRoundedPath(borderRect, 10))
            {
                using (Pen pen = new Pen(_borderColor, _borderWidth))
                {
                    g.DrawPath(pen, path);
                }
            }

            // Clear background behind text
            using (SolidBrush brushBack = new SolidBrush(_titleBackColor))
            {
                g.FillRectangle(brushBack, textRect.X - 2, textRect.Y, textRect.Width + 4, textRect.Height);
            }

            // Draw Text
            using (SolidBrush brushText = new SolidBrush(this.ForeColor))
            {
                g.DrawString(this.Text, this.Font, brushText, textRect.X, 0);
            }
        }

        private GraphicsPath GetRoundedPath(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            int d = radius * 2;

            if (d > rect.Height) d = rect.Height;
            if (d > rect.Width) d = rect.Width;

            path.StartFigure();
            path.AddArc(rect.X, rect.Y, d, d, 180, 90);
            path.AddArc(rect.Right - d, rect.Y, d, d, 270, 90);
            path.AddArc(rect.Right - d, rect.Bottom - d, d, d, 0, 90);
            path.AddArc(rect.X, rect.Bottom - d, d, d, 90, 90);
            path.CloseFigure();
            return path;
        }
    }
}
