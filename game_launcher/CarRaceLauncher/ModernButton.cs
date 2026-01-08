using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace CarRaceLauncher
{
    public class ModernButton : Button
    {
        private int _borderRadius = 40;
        private float _gradientAngle = 90F;
        private bool _isHovered = false;
        private bool _isPressed = false;
        private bool _isSelected = false;
        private Color _customBackColor = Color.MediumSlateBlue;

        public ModernButton()
        {
            this.SetStyle(ControlStyles.SupportsTransparentBackColor, true);
            this.SetStyle(ControlStyles.Opaque, false);
            this.SetStyle(ControlStyles.ResizeRedraw, true);

            this.FlatStyle = FlatStyle.Flat;
            this.FlatAppearance.BorderSize = 0;
            this.Size = new Size(150, 50);
            base.BackColor = Color.Transparent;
            this.ForeColor = Color.White;
            this.Font = new Font("Segoe UI", 12F, FontStyle.Bold);

            this.Resize += (s, e) => { if (_borderRadius > this.Height) _borderRadius = this.Height; this.Invalidate(); };
            this.MouseDown += (s, e) => { _isPressed = true; this.Invalidate(); };
            this.MouseUp += (s, e) => { _isPressed = false; this.Invalidate(); };
            this.MouseEnter += (s, e) => { _isHovered = true; this.Invalidate(); };
            this.MouseLeave += (s, e) => { _isHovered = false; this.Invalidate(); };
        }

        [Category("Appearance")]
        public override Color BackColor
        {
            get { return _customBackColor; }
            set
            {
                _customBackColor = value;
                base.BackColor = Color.Transparent;
                this.Invalidate();
            }
        }

        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                _isSelected = value;
                this.Invalidate();
            }
        }

        [Category("Appearance")]
        [DefaultValue(40)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public int BorderRadius
        {
            get { return _borderRadius; }
            set { _borderRadius = value; this.Invalidate(); }
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            base.OnPaintBackground(pevent);
        }

        protected override void OnPaint(PaintEventArgs pevent)
        {
            if (this.Width <= 0 || this.Height <= 0) return;

            Graphics g = pevent.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.PixelOffsetMode = PixelOffsetMode.HighQuality;
            g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.ClearTypeGridFit;

            // Defines the limits of the button
            RectangleF rectSurface = new RectangleF(0, 0, this.Width, this.Height);

            // Interpret BorderRadius as the "Corner Radius" (approx).
            // Actually usually diameter, but we can stick to diameter for GetFigurePath logic
            float diameter = _borderRadius * 2;
            if (diameter > rectSurface.Height) diameter = rectSurface.Height;

            // Create the path for the button shape
            using (GraphicsPath path = GetFigurePath(rectSurface, diameter))
            {
                // Set the Region to clip to the path (removes corners)
                this.Region = new Region(path);

                // 1. Calculate Colors for Gradient
                Color baseColor = _customBackColor;
                if (!this.Enabled) baseColor = Color.Gray;

                Color color1, color2;

                if (_isPressed)
                {
                    // Dark pressed look
                    color1 = ControlPaint.Dark(baseColor, 0.15f);
                    color2 = ControlPaint.Dark(baseColor, 0.45f);
                }
                else if (_isSelected)
                {
                    // Selected: Very bright/vivid gradient "Active"
                    color1 = ControlPaint.Light(baseColor, 0.6f);
                    color2 = ControlPaint.Light(baseColor, 0.1f);
                }
                else if (_isHovered)
                {
                    // Hover: Brighter than normal
                    color1 = ControlPaint.Light(baseColor, 0.3f);
                    color2 = ControlPaint.Dark(baseColor, 0.05f);
                }
                else
                {
                    // Normal
                    color1 = ControlPaint.Light(baseColor, 0.1f);
                    color2 = ControlPaint.Dark(baseColor, 0.2f);
                }

                // 2. Fill Background
                using (LinearGradientBrush brush = new LinearGradientBrush(rectSurface, color1, color2, _gradientAngle))
                {
                    g.FillPath(brush, path);
                }

                // 3. Draw Highlight Border (if Selected) or normal border
                if (_isSelected)
                {
                    // Thick White Border for Selection
                    using (Pen penHighlight = new Pen(Color.White, 4F))
                    {
                        penHighlight.Alignment = PenAlignment.Inset;
                        g.DrawPath(penHighlight, path);
                    }
                }
                else
                {
                    // Subtle dark border for definition
                    using (Pen penBorder = new Pen(ControlPaint.Dark(color2, 0.6f), 2F))
                    {
                        penBorder.Alignment = PenAlignment.Inset;
                        g.DrawPath(penBorder, path);
                    }
                }

                // 4. Draw Text
                SizeF textSize = g.MeasureString(this.Text, this.Font);
                PointF textPos = new PointF(
                    (this.Width - textSize.Width) / 2,
                    (this.Height - textSize.Height) / 2
                );

                using (SolidBrush textBrush = new SolidBrush(this.ForeColor))
                {
                    g.DrawString(this.Text, this.Font, textBrush, textPos.X, textPos.Y);
                }
            }
            // End of Region/Path usage
        }

        private GraphicsPath GetFigurePath(RectangleF rect, float diameter)
        {
            GraphicsPath path = new GraphicsPath();
            float d = diameter;

            // Should verify positive dimensions
            if (d <= 0.1f) d = 0.1f;

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
