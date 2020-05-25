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

namespace UI
{
    public partial class frmMain : Form
    {


        public frmMain()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            this.UpdateStyles();
        }

      
        private void FrmMain_Load(object sender, EventArgs e)
        {

            this.BackColor = Color.White;
        }
    
      


        private void MenuStrip2_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void 退出ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //StudentList.InputInfo();
            Close();
        }


        private void Button4_Click(object sender, EventArgs e)
        {
          
        }

        private void ToolStripStatusLabel2_Click(object sender, EventArgs e)
        {

        }

        private void ToolStripStatusLabel2_Click_1(object sender, EventArgs e)
        {

        }

        private void Label1_Click(object sender, EventArgs e)
        {

        }

        private void Label4_Click(object sender, EventArgs e)
        {

        }

        private void Label5_Click(object sender, EventArgs e)
        {

        }
    }
}
