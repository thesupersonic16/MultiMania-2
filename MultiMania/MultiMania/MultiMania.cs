using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MultiMania
{
    public class MultiMania
    {

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void Callback();

        private static string GetServerAddress()
        {
            var wc = new WebClient();
            string address = Encoding.ASCII.GetString(
                wc.DownloadData("http://multimania.codenamegamma.com/address.txt")
                );
            wc.Dispose();
            return address;
        }

        private static bool OnPacketRecv(byte[] data, IPEndPoint ip)
        {
            if (data[0] == 100)
            {
                string s = Encoding.ASCII.GetString(data, 1, data.Length - 1);
                //Test(s);
            }
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Connect(string connectionCode, int PPS)
        {
            MessageBox.Show("Connection Code: " + connectionCode);
            return false;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Host(int PPS)
        {
            MessageBox.Show("Host");
            return false;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static void InitMultiMania()
        {
            //var network = new Network();
            //network.OpenConnection(GetServerAddress(), 16004);
            //network.RegisterPacketEvent(OnPacketRecv);
            //network.SendCommand(100);
            //network.CloseConnection();
            //OpenMenu();
        }


        [DllImport("MultiMania-Mod.dll")]
        public static extern int OpenMenu();

    }
}
