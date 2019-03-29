using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MultiMania
{
    public class MultiMania
    {

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
                Test(s);
            }
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static void InitMultiMania()
        {
            var network = new Network();
            network.OpenConnection(GetServerAddress(), 16004);
            network.RegisterPacketEvent(OnPacketRecv);
            network.SendCommand(100);
            //network.CloseConnection();
        }


        [DllImport("MultiMania-Mod.dll")]
        public static extern int Test(string code);

    }
}
