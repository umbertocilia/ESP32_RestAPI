using System;
using System.Collections;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Threading.Tasks;
using System.Data;
using System.Linq;


namespace ESP32RestApiConsole
{


    public class StationInfo
    {
        public int return_value { get; set; }
        public int Id { get; set; }
        public string Barcode { get; set; }
        public int FormatoCollo { get; set; }
        public int Trigger { get; set; }
        public int EchoTrigger { get; set; }
        public int EventType { get; set; }
        public Boolean PresenzaLogica { get; set; }
        public int Events { get; set; }
    }

   

    class Program
    {
        static HttpClient client = new HttpClient();


        static void ShowStation(StationInfo staInfo)
        {
            Console.WriteLine($"ID: {staInfo.Id}\tBarcode: " + $"{staInfo.Barcode} " + $"FormatoCollo: {staInfo.FormatoCollo}\t Trigger:  " + $"{staInfo.Trigger} ");
        }


        static async Task<StationInfo> GetStationInfoAsync(string path)
        {
            StationInfo sta = null;
            HttpResponseMessage response = await client.GetAsync(path);
            if (response.IsSuccessStatusCode)
            {
                sta = await response.Content.ReadAsAsync<StationInfo>();
            }
            return sta;
        }


        static async Task<StationInfo> GetStationWithEventAsync()
        {
            string path = "GetStation?data=0";
            StationInfo sta = null;
            HttpResponseMessage response = await client.GetAsync(path);
            if (response.IsSuccessStatusCode)
            {
                sta = await response.Content.ReadAsAsync<StationInfo>();
            }

            int staIndex = (int)Math.Log(sta.Events, 2) + 1;

            sta = await GetStationInfoAsync(String.Format("GetStation?data={0}", staIndex));

            return sta;
        }


        static async Task<StationInfo> BalanceTriggerWithEventAsync(StationInfo sta)
        {
            string path = String.Format("BalanceTrigger?data={0}|{1}", sta.Id,sta.Trigger);
            
            HttpResponseMessage response = await client.GetAsync(path);
            if (response.IsSuccessStatusCode)
            {
                sta = await response.Content.ReadAsAsync<StationInfo>();
            }

            return sta;
        }


        static void Main(string[] args)
        {
            RunAsync(args[0]).GetAwaiter().GetResult();
        }

        static async Task RunAsync(String ESPIP)
        {
            // Update port # in the following line.
            client.BaseAddress = new Uri(ESPIP);
            client.DefaultRequestHeaders.Accept.Clear();
            client.DefaultRequestHeaders.Accept.Add(
                new MediaTypeWithQualityHeaderValue("application/json"));

            try
            {
                StationInfo sta = new StationInfo();

                sta = await GetStationWithEventAsync();

                while (sta.Events != 0)
                {
                    sta = await GetStationWithEventAsync();

                    ShowStation(sta);

                    sta = await BalanceTriggerWithEventAsync(sta);

                    sta = await GetStationWithEventAsync();


                }

                


            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            Console.ReadLine();
        }
    }
}
