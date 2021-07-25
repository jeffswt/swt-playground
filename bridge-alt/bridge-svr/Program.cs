using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

namespace BridgeAlt
{
    public class Program
    {
        public static void Main(string[] args)
        {
            // var req = new BridgeAlt.Models.LocationRequest();
            // req.loc = "/d/Development/github/jeffswt/swt-playground/bridge-alt";
            // (new BridgeAlt.Controllers.FileManagerController()).ListFolderInterface(req);
            CreateHostBuilder(args).Build().Run();
        }

        public static IHostBuilder CreateHostBuilder(string[] args) =>
            Host.CreateDefaultBuilder(args)
                .ConfigureWebHostDefaults(webBuilder =>
                {
                    webBuilder.UseStartup<Startup>();
                });
    }
}
