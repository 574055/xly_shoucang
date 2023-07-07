using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace WeshareGUI
{
    public delegate void d1(string indata);

    public partial class Form1 : Form
    {
        bool record = false;
        string my_file_path = "test.csv";
        public Form1()
        {
            InitializeComponent();
            
            WindowState = FormWindowState.Maximized;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            comboBox_Port.Items.AddRange(ports);

            chart1.Series["Weight"].Points.AddXY(1, 1);
            chart2.Series["Temp"].Points.AddXY(1, 1);
            chart2.Series["Hum"].Points.AddXY(1, 1);
        }

        
        public static void write_csv(string data, string file_path, bool save)
        {
            // writes to a csv file
            if (save)
            {
                try
                {
                    using (System.IO.StreamWriter file = new System.IO.StreamWriter(file_path, true))
                    {
                        file.Write(data + ",");
                    }
                }
                catch (Exception ex)
                {
                    throw new ApplicationException("This program encountered an error:", ex);
                }
            }
        }

        public static void writenl_csv(string data, string file_path, bool save)
        {
            // writes to a csv file and appends a finish line char
            if (save)
            {
                try
                {
                    using (System.IO.StreamWriter file = new System.IO.StreamWriter(file_path, true))
                    {
                        file.WriteLine(data);
                    }
                }
                catch (Exception ex)
                {
                    throw new ApplicationException("This program encountered an error:", ex);
                }
            }   
        }

        //conncet button was clicked
        private void button_CONNECT_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.PortName = comboBox_Port.Text;
                serialPort1.Open();
                chart1.Series["Weight"].Points.Clear();
                chart2.Series["Temp"].Points.Clear();
                chart2.Series["Hum"].Points.Clear();
                button_CONNECT.Text = "Connected";
                textBox_SSID.Text = "Loading...";
                textBox_weights.Text = "Loading...";
                textBox_numberOfTags.Text = "Loading...";
                textBox_scalefactor.Text = "Loading...";
                serialPort1.Write("C");
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //connect wifi
            string m1 = "S" + textBox_SSID.Text + ":;" + textBox_PWD.Text;
            serialPort1.Write(m1);
            button_connect_wifi.Text = "Saved";
        }

        int numberOfTags = 0;
        public void WriteForm(string indata)
        {
            // handles data sent from arduino
            char firstchar;
            Single numdata;
            firstchar = indata[0];
            
            switch (firstchar)
            {
                //case 'p':
                //    counter++;
                //    textBox2.Text = Convert.ToString(counter);
                //    break;
                //case 'u': //read staff ID
                //    textBox_StaffID.Text = Convert.ToString(indata.Substring(1));
                //    break;
                case 'v':
                    //numdata = Convert.ToSingle(indata.Substring(1));
                    textBox_PWD.Text = Convert.ToString(indata.Substring(1));
                    break;
                case '%': //read Wifi SSID
                    textBox_SSID.Text = Convert.ToString(indata.Substring(1));
                    break;
                case 'F': //read ScaleFactor
                    textBox_scalefactor.Text = Convert.ToString(indata.Substring(1));
                    break;
                case 'w': //read weights
                    double weight = Convert.ToDouble(indata.Substring(1));
                    
                    if (label1.Text == "Weight in [g]") //if gramm is selected
                    {
                        chart1.Series["Weight"].Points.Add(weight*1000);
                        textBox_weights.Text = Convert.ToString(weight * 1000);
                    }
                    else //if kg is selected
                    {
                        chart1.Series["Weight"].Points.Add(weight);
                        textBox_weights.Text = Convert.ToString(weight);
                    }
                    if (record) //write to csv
                    {
                        if (label1.Text == "Weight in [g]")
                        {
                            write_csv(Convert.ToString(weight * 1000), my_file_path, record);
                        }
                        else
                        {
                            write_csv(Convert.ToString(weight), my_file_path, record);
                        }
                    }
                    break;
                case 'n': //read number of Tags
                    textBox_numberOfTags.Text = Convert.ToString(indata.Substring(1));
                    numberOfTags = Convert.ToInt32(indata.Substring(1));
                    chart1.Series["nTags"].Points.Add(numberOfTags);
                    write_csv(Convert.ToString(numberOfTags), my_file_path, record);
                    break;
                case 't': //read temperatur
                    textBox_temp.Text = Convert.ToString(indata.Substring(1));
                    double temp = Convert.ToDouble(indata.Substring(1));
                    chart2.Series["Temp"].Points.Add(temp);
                    write_csv(Convert.ToString(temp), my_file_path, record);
                    break;
                case 'H': //read humidity
                    textBox_hum.Text = Convert.ToString(indata.Substring(1));
                    double hum = Convert.ToDouble(indata.Substring(1));
                    chart2.Series["Hum"].Points.Add(hum);
                    writenl_csv(Convert.ToString(hum), my_file_path, record);
                    break;
                case 'A': //read Connected
                    button_connect_wifi.BackColor = Color.Green;
                    break;
                case 'a': //read not connected
                    button_connect_wifi.BackColor = Color.Red;
                    break;
                case 'f': //read final tags
                    string finalTags = Convert.ToString(indata.Substring(1));
                    string tag = "";
                    textBox_StaffID.Text = "";
                    for (int i = 0; i < numberOfTags; i++)
                    {
                        if(finalTags.Substring(i * 8, 1) == "a") // if leading character is "a" it's a staff ID
                        {
                            textBox_StaffID.Text = Convert.ToString(finalTags.Substring(i * 8,8));
                        }
                        else // normal tag
                        {
                            tag += finalTags.Substring(4 + i * 8, 4) + ";"; //only last two digits                 

                        }
                    }
                    //for (int i = 0; i < numberOfTags; i++)
                    //{
                    //    tag += finalTags.Substring(4 + i * 8, 4) + ";"; //only last two digits                 
                    //}
                    textbox_finalTags.Text = tag;
                    break;
                case 'd': //read added Tags
                    string addedTags = Convert.ToString(indata.Substring(1));
                    int n_addedtags = addedTags.Length / 8;
                    string addtag = "";
                    for (int i = 0; i < n_addedtags; i++)
                    {
                        addtag += addedTags.Substring(4 + i * 8, 4) + ";"; //only last two digits                 
                    }
                    textBox_addedTags.Text = addtag;
                    break;
                case 'r': //read removed Tags
                    string removedTags = Convert.ToString(indata.Substring(1));
                    int n_removedtags = removedTags.Length / 8;
                    string remtag = "";
                    for (int i = 0; i < n_removedtags; i++)
                    {
                        remtag += removedTags.Substring(4 + i * 8, 4) + ";"; //only last two digits                 
                    }
                    textBox_removedTags.Text = remtag;
                    break;
                case 'L': //read Light Sensor
                    textBox_LightSensor.Text = Convert.ToString(indata.Substring(1));
                    break;
                case 'U': //read SerialNumber
                    textBox_SerialNumber.Text = Convert.ToString(indata.Substring(1));
                    break;
                case 'V': //read Version
                    textBox_version.Text = Convert.ToString(indata.Substring(1));
                    break;
            }
        }

        private void groupBox4_Enter(object sender, EventArgs e)
        {

        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string indata = serialPort1.ReadLine();
            d1 writeit = new d1(WriteForm);
            Invoke(writeit, indata);
        }

        private void button_save_calib_Click(object sender, EventArgs e)
        {
            //save Scalefactor
            string m1 = "F" + textBox_scalefactor.Text;
            serialPort1.Write(m1);
            button_save_calib.Text = "Saved";
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            //tare button
            serialPort1.Write("t");
        }

        private void button_clear_Click(object sender, EventArgs e)
        {
            foreach (var series in chart1.Series)
            {
                series.Points.Clear();
            }
            foreach (var series in chart2.Series)
            {
                series.Points.Clear();
            }
            chart1.Series["Weight"].Points.AddXY(0, 0);
            chart1.Series["nTags"].Points.AddXY(0, 0);
            chart2.Series["Temp"].Points.AddXY(0, 0);
            chart2.Series["Hum"].Points.AddXY(0, 0);
        }


        private void label4_Click(object sender, EventArgs e)
        {

        }



        private void chart2_Click(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {

        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        
        private void button2_Click(object sender, EventArgs e)
        {
            // start recording
            my_file_path = textBox_data.Text + ".csv";
            record = !record;
            if(record)
            {
                try
                {
                    using (System.IO.StreamWriter file = new System.IO.StreamWriter(my_file_path, true))
                    {
                        file.WriteLine("Weight,Number of Tags,Temperature,Humidity");
                    }
                }
                catch (Exception ex)
                {
                    throw new ApplicationException("This program encountered an error:", ex);
                }
                button_record.Text = "Stop Recording";
            }
            else
            {
                button_record.Text = "Start Recording";
            }
        }

        private void textBox_hum_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label11_Click(object sender, EventArgs e)
        {

        }

        private void button_auto_calib_Click(object sender, EventArgs e)
        {
            //send auto calibration command
            if (label1.Text == "Weight in [g]")
            {
                string m1 = "s" + textBox_calib.Text;
                serialPort1.Write(m1);
            }
            else
            {
                double m1 = Convert.ToDouble(textBox_calib.Text) * 1000;
                string m2 = "s" + Convert.ToString(m1);
                serialPort1.Write(m2);
            }
            
            
        }

        private void label12_Click(object sender, EventArgs e)
        {

        }

        private void comboBox_Port_SelectedIndexChanged(object sender, EventArgs e)
        {
            
        }

        private void comboBox_units_SelectedIndexChanged(object sender, EventArgs e)
        {
            label1.Text = "Weight in [" + comboBox_units.Text + "]";
            label11.Text = "calibration weight [" + comboBox_units.Text + "]";
            this.chart1.Titles["Title1"].Text = "Weight in [" + comboBox_units.Text + "]";
            

        }

        private void textBox_StaffID_TextChanged(object sender, EventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string power = "p" + comboBox1.SelectedIndex.ToString();
            serialPort1.Write(power);

        }

        private void button_DISCONNECT_Click(object sender, EventArgs e)
        {
            serialPort1.Close();
            serialPort1.Dispose();
        }

        private void button_scan_ports_Click(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            comboBox_Port.Items.AddRange(ports);
        }

        private void textBox2_TextChanged_1(object sender, EventArgs e)
        {
            //read Light sensor

        }

        private void textBox_SerialNumber_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
