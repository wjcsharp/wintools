using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace EventLogParser
{
    public partial class MainForm : Form
    {
        private delegate void ShowMessageHandler(string msg);
        private delegate void ShowProgressHandler(int val, int max);

        ShowMessageHandler msgHandler;
        ShowProgressHandler pbHandler;

        DataSet ds;
        BindingSource bs;

        public MainForm()
        {
            InitializeComponent();
            msgHandler = new ShowMessageHandler(ShowMsg);
            pbHandler = new ShowProgressHandler(UpdatePb);
        }

        // Exit the application
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFile();
        }

        // Open the log file
        private void OpenFile()
        {
            // Show file open dialog
            if (openFile.ShowDialog() == DialogResult.OK)
            {
                // Create a dataset for binding the data to the grid.
                ds = new DataSet("EventLog Entries");
                ds.Tables.Add("Events");
                ds.Tables["Events"].Columns.Add("Type");
                ds.Tables["Events"].Columns.Add("Date");
                ds.Tables["Events"].Columns.Add("Time");
                ds.Tables["Events"].Columns.Add("Source");
                ds.Tables["Events"].Columns.Add("Description");
                ds.Tables["Events"].Columns.Add("Category");
                ds.Tables["Events"].Columns.Add("EventID");
                ds.Tables["Events"].Columns.Add("User");
                ds.Tables["Events"].Columns.Add("System");
                // Start the processing as a background process
                worker.RunWorkerAsync(openFile.FileName);
            }
        }

        // Update the value of the progress bar.
        void UpdatePb(int val, int max)
        {
            this.pb.Value = (val * 100) / max;
        }

        // Event: A new event log record is parsed.
        void parser_OnFoundRecord(object[] items)
        {
            ds.Tables[0].Rows.Add(items);
        }

        // Event: Showing the progress 
        void parser_OnProgress(int val, int max)
        {
            this.Invoke(pbHandler, new object[] { val, max });
        }

        // Update the status bar message
        private void ShowMsg(string msg)
        {
            this.toolStripStatusLabel1.Text = msg;
        }

        // Event: Show the status bar message
        void parser_OnAction(string msg)
        {
            this.Invoke(msgHandler, new object[] { msg });
        }

        // Start the parsing of the file
        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            EventLogParser parser = new EventLogParser();
            parser.OnAction += new MessageHandler(parser_OnAction);
            parser.OnProgress += new ProgressHandler(parser_OnProgress);
            parser.OnFoundRecord += new NewEventFoundHandler(parser_OnFoundRecord);
            parser.Parse(e.Argument.ToString());
        }

        // Finished the parsing of the file.
        // Bind the dataset to the grid.
        private void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            bs = new BindingSource(ds, "Events");
            dataGridView1.DataSource = bs;
            this.Invoke(pbHandler, new object[] { 100, 100 });
        }

        private void openLogFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFile();
        }

        // Exit the application
        private void exitToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        // Show the description in a message box.
        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                DataGridViewRow dv = this.dataGridView1.SelectedRows[0];
                if (dv != null)
                {
                    MessageBox.Show(this, dv.Cells["Description"].Value.ToString().Replace('\0', '\n'), "Description", MessageBoxButtons.OK, MessageBoxIcon.None);
                }
            }
            catch (Exception)
            {
            }
        }

        // Select the row when a cell is clicked
        private void dataGridView1_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                if (e.RowIndex == -1)
                    return;
                dataGridView1.Rows[e.RowIndex].Selected = true;
            }
            catch (Exception)
            {
            }
        }
    }
}