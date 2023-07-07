
using System.Windows.Forms;

namespace WeshareGUI
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.Title title3 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title4 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.Title title5 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.Title title6 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_scan_ports = new System.Windows.Forms.Button();
            this.button_DISCONNECT = new System.Windows.Forms.Button();
            this.button_CONNECT = new System.Windows.Forms.Button();
            this.comboBox_Port = new System.Windows.Forms.ComboBox();
            this.label_com = new System.Windows.Forms.Label();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label18 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.textBox5 = new System.Windows.Forms.TextBox();
            this.button_connect_wifi = new System.Windows.Forms.Button();
            this.label_PWD = new System.Windows.Forms.Label();
            this.label_SSID = new System.Windows.Forms.Label();
            this.textBox_PWD = new System.Windows.Forms.TextBox();
            this.textBox_SSID = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label11 = new System.Windows.Forms.Label();
            this.textBox_calib = new System.Windows.Forms.TextBox();
            this.button_auto_calib = new System.Windows.Forms.Button();
            this.button_clear = new System.Windows.Forms.Button();
            this.button_tare = new System.Windows.Forms.Button();
            this.button_save_calib = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_scalefactor = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label12 = new System.Windows.Forms.Label();
            this.comboBox_units = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_numberOfTags = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_weights = new System.Windows.Forms.TextBox();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.textbox_finalTags = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_addedTags = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.textBox_removedTags = new System.Windows.Forms.TextBox();
            this.chart2 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.textBox_hum = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.textBox_temp = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.label9 = new System.Windows.Forms.Label();
            this.button_record = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.textBox_data = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.textBox_StaffID = new System.Windows.Forms.TextBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.label15 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.label16 = new System.Windows.Forms.Label();
            this.textBox_LightSensor = new System.Windows.Forms.TextBox();
            this.groupBox9 = new System.Windows.Forms.GroupBox();
            this.label14 = new System.Windows.Forms.Label();
            this.textBox_version = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.textBox_SerialNumber = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart2)).BeginInit();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox9.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button_scan_ports);
            this.groupBox1.Controls.Add(this.button_DISCONNECT);
            this.groupBox1.Controls.Add(this.button_CONNECT);
            this.groupBox1.Controls.Add(this.comboBox_Port);
            this.groupBox1.Controls.Add(this.label_com);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.Location = new System.Drawing.Point(47, 38);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(406, 173);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Serial connection";
            // 
            // button_scan_ports
            // 
            this.button_scan_ports.BackColor = System.Drawing.SystemColors.Control;
            this.button_scan_ports.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_scan_ports.Location = new System.Drawing.Point(197, 74);
            this.button_scan_ports.Name = "button_scan_ports";
            this.button_scan_ports.Size = new System.Drawing.Size(203, 41);
            this.button_scan_ports.TabIndex = 4;
            this.button_scan_ports.Text = "SCAN PORTS";
            this.button_scan_ports.UseVisualStyleBackColor = false;
            this.button_scan_ports.Click += new System.EventHandler(this.button_scan_ports_Click);
            // 
            // button_DISCONNECT
            // 
            this.button_DISCONNECT.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(224)))), ((int)(((byte)(192)))));
            this.button_DISCONNECT.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_DISCONNECT.Location = new System.Drawing.Point(6, 123);
            this.button_DISCONNECT.Name = "button_DISCONNECT";
            this.button_DISCONNECT.Size = new System.Drawing.Size(185, 41);
            this.button_DISCONNECT.TabIndex = 3;
            this.button_DISCONNECT.Text = "DISCONNECT";
            this.button_DISCONNECT.UseVisualStyleBackColor = false;
            this.button_DISCONNECT.Click += new System.EventHandler(this.button_DISCONNECT_Click);
            // 
            // button_CONNECT
            // 
            this.button_CONNECT.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.button_CONNECT.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_CONNECT.Location = new System.Drawing.Point(6, 74);
            this.button_CONNECT.Name = "button_CONNECT";
            this.button_CONNECT.Size = new System.Drawing.Size(185, 41);
            this.button_CONNECT.TabIndex = 2;
            this.button_CONNECT.Text = "CONNECT";
            this.button_CONNECT.UseVisualStyleBackColor = false;
            this.button_CONNECT.Click += new System.EventHandler(this.button_CONNECT_Click);
            // 
            // comboBox_Port
            // 
            this.comboBox_Port.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_Port.FormattingEnabled = true;
            this.comboBox_Port.Location = new System.Drawing.Point(142, 32);
            this.comboBox_Port.Name = "comboBox_Port";
            this.comboBox_Port.Size = new System.Drawing.Size(121, 33);
            this.comboBox_Port.TabIndex = 1;
            this.comboBox_Port.SelectedIndexChanged += new System.EventHandler(this.comboBox_Port_SelectedIndexChanged);
            // 
            // label_com
            // 
            this.label_com.AutoSize = true;
            this.label_com.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_com.Location = new System.Drawing.Point(22, 32);
            this.label_com.Name = "label_com";
            this.label_com.Size = new System.Drawing.Size(100, 25);
            this.label_com.TabIndex = 0;
            this.label_com.Text = "COM Port";
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label18);
            this.groupBox2.Controls.Add(this.label19);
            this.groupBox2.Controls.Add(this.textBox4);
            this.groupBox2.Controls.Add(this.textBox5);
            this.groupBox2.Controls.Add(this.button_connect_wifi);
            this.groupBox2.Controls.Add(this.label_PWD);
            this.groupBox2.Controls.Add(this.label_SSID);
            this.groupBox2.Controls.Add(this.textBox_PWD);
            this.groupBox2.Controls.Add(this.textBox_SSID);
            this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox2.Location = new System.Drawing.Point(1424, 38);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(390, 341);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Wifi";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label18.Location = new System.Drawing.Point(30, 249);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(94, 25);
            this.label18.TabIndex = 8;
            this.label18.Text = "IP adress";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.Location = new System.Drawing.Point(6, 196);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(125, 25);
            this.label19.TabIndex = 7;
            this.label19.Text = "MAC Adress";
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(198, 249);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(182, 30);
            this.textBox4.TabIndex = 6;
            // 
            // textBox5
            // 
            this.textBox5.Location = new System.Drawing.Point(198, 196);
            this.textBox5.Name = "textBox5";
            this.textBox5.Size = new System.Drawing.Size(182, 30);
            this.textBox5.TabIndex = 5;
            // 
            // button_connect_wifi
            // 
            this.button_connect_wifi.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.button_connect_wifi.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_connect_wifi.Location = new System.Drawing.Point(198, 121);
            this.button_connect_wifi.Name = "button_connect_wifi";
            this.button_connect_wifi.Size = new System.Drawing.Size(144, 41);
            this.button_connect_wifi.TabIndex = 4;
            this.button_connect_wifi.Text = "CONNECT";
            this.button_connect_wifi.UseVisualStyleBackColor = false;
            this.button_connect_wifi.Click += new System.EventHandler(this.button1_Click);
            // 
            // label_PWD
            // 
            this.label_PWD.AutoSize = true;
            this.label_PWD.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_PWD.Location = new System.Drawing.Point(30, 87);
            this.label_PWD.Name = "label_PWD";
            this.label_PWD.Size = new System.Drawing.Size(98, 25);
            this.label_PWD.TabIndex = 3;
            this.label_PWD.Text = "Password";
            // 
            // label_SSID
            // 
            this.label_SSID.AutoSize = true;
            this.label_SSID.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_SSID.Location = new System.Drawing.Point(6, 34);
            this.label_SSID.Name = "label_SSID";
            this.label_SSID.Size = new System.Drawing.Size(165, 25);
            this.label_SSID.TabIndex = 2;
            this.label_SSID.Text = "SSID (Wifi name)";
            // 
            // textBox_PWD
            // 
            this.textBox_PWD.Location = new System.Drawing.Point(198, 87);
            this.textBox_PWD.Name = "textBox_PWD";
            this.textBox_PWD.Size = new System.Drawing.Size(182, 30);
            this.textBox_PWD.TabIndex = 1;
            // 
            // textBox_SSID
            // 
            this.textBox_SSID.Location = new System.Drawing.Point(198, 34);
            this.textBox_SSID.Name = "textBox_SSID";
            this.textBox_SSID.Size = new System.Drawing.Size(182, 30);
            this.textBox_SSID.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.textBox_calib);
            this.groupBox3.Controls.Add(this.button_auto_calib);
            this.groupBox3.Controls.Add(this.button_clear);
            this.groupBox3.Controls.Add(this.button_tare);
            this.groupBox3.Controls.Add(this.button_save_calib);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.textBox_scalefactor);
            this.groupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox3.Location = new System.Drawing.Point(334, 217);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(557, 162);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Scale calibration";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(351, 26);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(199, 25);
            this.label11.TabIndex = 9;
            this.label11.Text = "calibration weight [kg]";
            this.label11.Click += new System.EventHandler(this.label11_Click);
            // 
            // textBox_calib
            // 
            this.textBox_calib.Location = new System.Drawing.Point(412, 57);
            this.textBox_calib.Name = "textBox_calib";
            this.textBox_calib.Size = new System.Drawing.Size(100, 30);
            this.textBox_calib.TabIndex = 8;
            this.textBox_calib.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // button_auto_calib
            // 
            this.button_auto_calib.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_auto_calib.Location = new System.Drawing.Point(412, 111);
            this.button_auto_calib.Name = "button_auto_calib";
            this.button_auto_calib.Size = new System.Drawing.Size(139, 45);
            this.button_auto_calib.TabIndex = 7;
            this.button_auto_calib.Text = "Auto Calib";
            this.button_auto_calib.UseVisualStyleBackColor = true;
            this.button_auto_calib.Click += new System.EventHandler(this.button_auto_calib_Click);
            // 
            // button_clear
            // 
            this.button_clear.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_clear.Location = new System.Drawing.Point(19, 111);
            this.button_clear.Name = "button_clear";
            this.button_clear.Size = new System.Drawing.Size(101, 45);
            this.button_clear.TabIndex = 6;
            this.button_clear.Text = "Clear";
            this.button_clear.UseVisualStyleBackColor = true;
            this.button_clear.Click += new System.EventHandler(this.button_clear_Click);
            // 
            // button_tare
            // 
            this.button_tare.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_tare.Location = new System.Drawing.Point(157, 111);
            this.button_tare.Name = "button_tare";
            this.button_tare.Size = new System.Drawing.Size(101, 45);
            this.button_tare.TabIndex = 5;
            this.button_tare.Text = "Tare";
            this.button_tare.UseVisualStyleBackColor = true;
            this.button_tare.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // button_save_calib
            // 
            this.button_save_calib.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_save_calib.Location = new System.Drawing.Point(283, 111);
            this.button_save_calib.Name = "button_save_calib";
            this.button_save_calib.Size = new System.Drawing.Size(101, 45);
            this.button_save_calib.TabIndex = 4;
            this.button_save_calib.Text = "Save";
            this.button_save_calib.UseVisualStyleBackColor = true;
            this.button_save_calib.Click += new System.EventHandler(this.button_save_calib_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(198, 26);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(115, 25);
            this.label2.TabIndex = 2;
            this.label2.Text = "Scale factor";
            // 
            // textBox_scalefactor
            // 
            this.textBox_scalefactor.Location = new System.Drawing.Point(212, 57);
            this.textBox_scalefactor.Name = "textBox_scalefactor";
            this.textBox_scalefactor.Size = new System.Drawing.Size(100, 30);
            this.textBox_scalefactor.TabIndex = 0;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.comboBox_units);
            this.groupBox4.Controls.Add(this.label3);
            this.groupBox4.Controls.Add(this.textBox_numberOfTags);
            this.groupBox4.Controls.Add(this.label1);
            this.groupBox4.Controls.Add(this.textBox_weights);
            this.groupBox4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox4.Location = new System.Drawing.Point(47, 217);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(281, 162);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Scale";
            this.groupBox4.Enter += new System.EventHandler(this.groupBox4_Enter);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(82, 29);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(56, 25);
            this.label12.TabIndex = 5;
            this.label12.Text = "Units";
            this.label12.Click += new System.EventHandler(this.label12_Click);
            // 
            // comboBox_units
            // 
            this.comboBox_units.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_units.FormattingEnabled = true;
            this.comboBox_units.Items.AddRange(new object[] {
            "kg",
            "g"});
            this.comboBox_units.Location = new System.Drawing.Point(154, 26);
            this.comboBox_units.Name = "comboBox_units";
            this.comboBox_units.Size = new System.Drawing.Size(121, 33);
            this.comboBox_units.TabIndex = 3;
            this.comboBox_units.SelectedIndexChanged += new System.EventHandler(this.comboBox_units_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(6, 125);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(152, 25);
            this.label3.TabIndex = 4;
            this.label3.Text = "Number of Tags";
            // 
            // textBox_numberOfTags
            // 
            this.textBox_numberOfTags.Location = new System.Drawing.Point(175, 125);
            this.textBox_numberOfTags.Name = "textBox_numberOfTags";
            this.textBox_numberOfTags.Size = new System.Drawing.Size(100, 30);
            this.textBox_numberOfTags.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(6, 71);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(132, 25);
            this.label1.TabIndex = 2;
            this.label1.Text = "Weight in [kg]";
            // 
            // textBox_weights
            // 
            this.textBox_weights.Location = new System.Drawing.Point(175, 71);
            this.textBox_weights.Name = "textBox_weights";
            this.textBox_weights.Size = new System.Drawing.Size(100, 30);
            this.textBox_weights.TabIndex = 0;
            // 
            // chart1
            // 
            chartArea1.AxisY.Title = "weight";
            chartArea1.AxisY2.Interval = 1D;
            chartArea1.AxisY2.MajorGrid.LineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea1.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea1);
            legend1.Name = "Legend1";
            this.chart1.Legends.Add(legend1);
            this.chart1.Location = new System.Drawing.Point(47, 385);
            this.chart1.Name = "chart1";
            series1.BorderWidth = 4;
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series1.Legend = "Legend1";
            series1.Name = "Weight";
            series2.BorderWidth = 2;
            series2.ChartArea = "ChartArea1";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series2.Legend = "Legend1";
            series2.Name = "nTags";
            series2.YAxisType = System.Windows.Forms.DataVisualization.Charting.AxisType.Secondary;
            this.chart1.Series.Add(series1);
            this.chart1.Series.Add(series2);
            this.chart1.Size = new System.Drawing.Size(693, 260);
            this.chart1.TabIndex = 7;
            this.chart1.Text = "chart1";
            title1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            title1.Name = "Title1";
            title1.Text = "Weight [kg]";
            title2.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Right;
            title2.Name = "nTags";
            title2.Text = "nTags";
            title3.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
            title3.Name = "n";
            title3.Text = "n measurements";
            this.chart1.Titles.Add(title1);
            this.chart1.Titles.Add(title2);
            this.chart1.Titles.Add(title3);
            // 
            // textbox_finalTags
            // 
            this.textbox_finalTags.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textbox_finalTags.Location = new System.Drawing.Point(47, 760);
            this.textbox_finalTags.Multiline = true;
            this.textbox_finalTags.Name = "textbox_finalTags";
            this.textbox_finalTags.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textbox_finalTags.Size = new System.Drawing.Size(1392, 59);
            this.textbox_finalTags.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(42, 732);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(85, 25);
            this.label4.TabIndex = 5;
            this.label4.Text = "Tag ID\'s";
            this.label4.Click += new System.EventHandler(this.label4_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(42, 832);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(117, 25);
            this.label5.TabIndex = 8;
            this.label5.Text = "added Tags";
            // 
            // textBox_addedTags
            // 
            this.textBox_addedTags.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_addedTags.Location = new System.Drawing.Point(47, 860);
            this.textBox_addedTags.Name = "textBox_addedTags";
            this.textBox_addedTags.Size = new System.Drawing.Size(1392, 30);
            this.textBox_addedTags.TabIndex = 9;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(42, 896);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(138, 25);
            this.label6.TabIndex = 10;
            this.label6.Text = "removed Tags";
            // 
            // textBox_removedTags
            // 
            this.textBox_removedTags.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_removedTags.Location = new System.Drawing.Point(47, 924);
            this.textBox_removedTags.Name = "textBox_removedTags";
            this.textBox_removedTags.Size = new System.Drawing.Size(1392, 30);
            this.textBox_removedTags.TabIndex = 11;
            // 
            // chart2
            // 
            chartArea2.AxisX.MajorGrid.Interval = 0D;
            chartArea2.AxisX2.MajorGrid.Interval = 0D;
            chartArea2.AxisX2.MinorGrid.Interval = 10D;
            chartArea2.AxisY.MajorGrid.Interval = 0D;
            chartArea2.AxisY.Title = "Temperature";
            chartArea2.AxisY2.Interval = 10D;
            chartArea2.AxisY2.MajorGrid.Enabled = false;
            chartArea2.AxisY2.MajorGrid.LineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea2.Name = "ChartArea1";
            this.chart2.ChartAreas.Add(chartArea2);
            legend2.Name = "Legend1";
            this.chart2.Legends.Add(legend2);
            this.chart2.Location = new System.Drawing.Point(746, 385);
            this.chart2.Name = "chart2";
            series3.BorderWidth = 4;
            series3.ChartArea = "ChartArea1";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series3.Color = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(0)))));
            series3.Legend = "Legend1";
            series3.Name = "Temp";
            series4.BorderWidth = 2;
            series4.ChartArea = "ChartArea1";
            series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series4.Color = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            series4.Legend = "Legend1";
            series4.Name = "Hum";
            series4.YAxisType = System.Windows.Forms.DataVisualization.Charting.AxisType.Secondary;
            this.chart2.Series.Add(series3);
            this.chart2.Series.Add(series4);
            this.chart2.Size = new System.Drawing.Size(693, 260);
            this.chart2.TabIndex = 12;
            this.chart2.Text = "chart2";
            title4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            title4.Name = "Title1";
            title4.Text = "Temperature[C]";
            title5.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Right;
            title5.Name = "nTags";
            title5.Text = "Hum";
            title6.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
            title6.Name = "n";
            title6.Text = "n measurements";
            this.chart2.Titles.Add(title4);
            this.chart2.Titles.Add(title5);
            this.chart2.Titles.Add(title6);
            this.chart2.Click += new System.EventHandler(this.chart2_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.groupBox5.Controls.Add(this.label7);
            this.groupBox5.Controls.Add(this.textBox_hum);
            this.groupBox5.Controls.Add(this.label8);
            this.groupBox5.Controls.Add(this.textBox_temp);
            this.groupBox5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox5.Location = new System.Drawing.Point(1066, 38);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(310, 173);
            this.groupBox5.TabIndex = 13;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "DHT11 Sensor";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(6, 111);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(122, 25);
            this.label7.TabIndex = 4;
            this.label7.Text = "Humidity [%]";
            // 
            // textBox_hum
            // 
            this.textBox_hum.Location = new System.Drawing.Point(204, 111);
            this.textBox_hum.Name = "textBox_hum";
            this.textBox_hum.Size = new System.Drawing.Size(100, 30);
            this.textBox_hum.TabIndex = 3;
            this.textBox_hum.TextChanged += new System.EventHandler(this.textBox_hum_TextChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(6, 57);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(156, 25);
            this.label8.TabIndex = 2;
            this.label8.Text = "Temperature [C]";
            // 
            // textBox_temp
            // 
            this.textBox_temp.Location = new System.Drawing.Point(204, 54);
            this.textBox_temp.Name = "textBox_temp";
            this.textBox_temp.Size = new System.Drawing.Size(100, 30);
            this.textBox_temp.TabIndex = 0;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.label9);
            this.groupBox6.Controls.Add(this.button_record);
            this.groupBox6.Controls.Add(this.label10);
            this.groupBox6.Controls.Add(this.textBox_data);
            this.groupBox6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox6.Location = new System.Drawing.Point(459, 38);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(601, 173);
            this.groupBox6.TabIndex = 14;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "CSV";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(6, 104);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(561, 25);
            this.label9.TabIndex = 8;
            this.label9.Text = "Data is saved in \"WeshareGUI\\bin\\Release\\<Your File Name>\" ";
            this.label9.Click += new System.EventHandler(this.label9_Click);
            // 
            // button_record
            // 
            this.button_record.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_record.Location = new System.Drawing.Point(372, 22);
            this.button_record.Name = "button_record";
            this.button_record.Size = new System.Drawing.Size(170, 45);
            this.button_record.TabIndex = 7;
            this.button_record.Text = "Start";
            this.button_record.UseVisualStyleBackColor = true;
            this.button_record.Click += new System.EventHandler(this.button2_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(11, 29);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(100, 25);
            this.label10.TabIndex = 2;
            this.label10.Text = "File Name";
            // 
            // textBox_data
            // 
            this.textBox_data.Location = new System.Drawing.Point(117, 29);
            this.textBox_data.Name = "textBox_data";
            this.textBox_data.Size = new System.Drawing.Size(240, 30);
            this.textBox_data.TabIndex = 0;
            this.textBox_data.Text = "data";
            this.textBox_data.TextChanged += new System.EventHandler(this.textBox2_TextChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(42, 662);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(76, 25);
            this.label13.TabIndex = 15;
            this.label13.Text = "Staff ID";
            // 
            // textBox_StaffID
            // 
            this.textBox_StaffID.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_StaffID.Location = new System.Drawing.Point(47, 690);
            this.textBox_StaffID.Name = "textBox_StaffID";
            this.textBox_StaffID.Size = new System.Drawing.Size(693, 30);
            this.textBox_StaffID.TabIndex = 16;
            this.textBox_StaffID.TextChanged += new System.EventHandler(this.textBox_StaffID_TextChanged);
            // 
            // groupBox7
            // 
            this.groupBox7.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.groupBox7.Controls.Add(this.comboBox1);
            this.groupBox7.Controls.Add(this.label15);
            this.groupBox7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox7.Location = new System.Drawing.Point(1445, 492);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(369, 116);
            this.groupBox7.TabIndex = 14;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "RFID reader";
            // 
            // comboBox1
            // 
            this.comboBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Items.AddRange(new object[] {
            "26",
            "20",
            "18.5",
            "15.5"});
            this.comboBox1.Location = new System.Drawing.Point(204, 54);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(92, 33);
            this.comboBox1.TabIndex = 4;
            this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label15.Location = new System.Drawing.Point(6, 57);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(155, 25);
            this.label15.TabIndex = 2;
            this.label15.Text = "set Power [dBm]";
            // 
            // groupBox8
            // 
            this.groupBox8.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.groupBox8.Controls.Add(this.label16);
            this.groupBox8.Controls.Add(this.textBox_LightSensor);
            this.groupBox8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox8.Location = new System.Drawing.Point(1445, 385);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(369, 101);
            this.groupBox8.TabIndex = 14;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Light Sensor";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label16.Location = new System.Drawing.Point(6, 57);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(122, 25);
            this.label16.TabIndex = 2;
            this.label16.Text = "Light Sensor";
            // 
            // textBox_LightSensor
            // 
            this.textBox_LightSensor.Location = new System.Drawing.Point(204, 54);
            this.textBox_LightSensor.Name = "textBox_LightSensor";
            this.textBox_LightSensor.Size = new System.Drawing.Size(100, 30);
            this.textBox_LightSensor.TabIndex = 0;
            this.textBox_LightSensor.TextChanged += new System.EventHandler(this.textBox2_TextChanged_1);
            // 
            // groupBox9
            // 
            this.groupBox9.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.groupBox9.Controls.Add(this.label14);
            this.groupBox9.Controls.Add(this.textBox_version);
            this.groupBox9.Controls.Add(this.label17);
            this.groupBox9.Controls.Add(this.textBox_SerialNumber);
            this.groupBox9.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox9.Location = new System.Drawing.Point(897, 217);
            this.groupBox9.Name = "groupBox9";
            this.groupBox9.Size = new System.Drawing.Size(521, 162);
            this.groupBox9.TabIndex = 17;
            this.groupBox9.TabStop = false;
            this.groupBox9.Text = "VERSION";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label14.Location = new System.Drawing.Point(6, 111);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(79, 25);
            this.label14.TabIndex = 4;
            this.label14.Text = "Verison";
            // 
            // textBox_version
            // 
            this.textBox_version.Location = new System.Drawing.Point(150, 111);
            this.textBox_version.Name = "textBox_version";
            this.textBox_version.Size = new System.Drawing.Size(154, 30);
            this.textBox_version.TabIndex = 3;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(6, 57);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(128, 25);
            this.label17.TabIndex = 2;
            this.label17.Text = "Serialnumber";
            // 
            // textBox_SerialNumber
            // 
            this.textBox_SerialNumber.Location = new System.Drawing.Point(150, 54);
            this.textBox_SerialNumber.Name = "textBox_SerialNumber";
            this.textBox_SerialNumber.Size = new System.Drawing.Size(365, 30);
            this.textBox_SerialNumber.TabIndex = 0;
            this.textBox_SerialNumber.TextChanged += new System.EventHandler(this.textBox_SerialNumber_TextChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1846, 933);
            this.Controls.Add(this.groupBox9);
            this.Controls.Add(this.groupBox8);
            this.Controls.Add(this.groupBox7);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.textBox_StaffID);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.chart2);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.textBox_removedTags);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.textBox_addedTags);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textbox_finalTags);
            this.Controls.Add(this.chart1);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "WeShare Tech Limited";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart2)).EndInit();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox8.ResumeLayout(false);
            this.groupBox8.PerformLayout();
            this.groupBox9.ResumeLayout(false);
            this.groupBox9.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button_CONNECT;
        private System.Windows.Forms.ComboBox comboBox_Port;
        private System.Windows.Forms.Label label_com;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button button_connect_wifi;
        private System.Windows.Forms.Label label_PWD;
        private System.Windows.Forms.Label label_SSID;
        private System.Windows.Forms.TextBox textBox_PWD;
        private System.Windows.Forms.TextBox textBox_SSID;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button button_save_calib;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_scalefactor;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_weights;
        private System.Windows.Forms.Button button_tare;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox_numberOfTags;
        private System.Windows.Forms.Button button_clear;
        private System.Windows.Forms.TextBox textbox_finalTags;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox_addedTags;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBox_removedTags;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart2;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox textBox_hum;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox textBox_temp;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Button button_record;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox textBox_data;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button button_auto_calib;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox textBox_calib;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox comboBox_units;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox textBox_StaffID;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Label label15;
        private Button button_DISCONNECT;
        private Button button_scan_ports;
        private Label label18;
        private Label label19;
        private TextBox textBox4;
        private TextBox textBox5;
        private GroupBox groupBox8;
        private Label label16;
        private TextBox textBox_LightSensor;
        private GroupBox groupBox9;
        private Label label14;
        private TextBox textBox_version;
        private Label label17;
        private TextBox textBox_SerialNumber;
    }
}

