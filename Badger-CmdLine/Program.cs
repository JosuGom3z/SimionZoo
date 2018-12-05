﻿using System;
using System.Collections.Generic;
using System.Threading;
using Herd.Files;
using Herd.Network;

namespace Badger_CmdLine
{
    class Program
    {
        static int numExperimentalUnitsLoaded = 0;
        static void OnExpUnitLoaded()
        {
            numExperimentalUnitsLoaded++;
        }

        static void Main(string[] args)
        {
            if (args.Length!=1)
            {
                Console.WriteLine("Wrong number of arguments. Usage:\nBadger-CmdLine.exe <batchFilename>");
                return;
            }
            string batchFilename= args[0];
            LoadOptions loadOptions = new LoadOptions()
            {
                LoadUnfinishedExpUnits = true,
                LoadFinishedExpUnits = false,
                LoadVariablesInLog = false,
                OnExpUnitLoaded = OnExpUnitLoaded
            };

            //load the experiment batch
            Console.WriteLine("Reading batch file: " + batchFilename);
            LoggedExperimentBatch batch = new LoggedExperimentBatch(batchFilename, loadOptions);
            Console.WriteLine("{0} unfinished experimental units loaded", numExperimentalUnitsLoaded);

            Console.WriteLine("Finding herd agents");
            Shepherd shepherd = new Shepherd();
            shepherd.CallHerd();

            Thread.Sleep(2000);

            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>();
            shepherd.GetHerdAgentList(ref herdAgents);

            Console.WriteLine("Found {0} herd agents", herdAgents.Count);

            int i = 0;
            foreach (HerdAgentInfo herdAgent in herdAgents)
                Console.WriteLine("#{0}: {1}", i, herdAgent.ipAddressString);
        }
    }
}