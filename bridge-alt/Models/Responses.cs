
using System.Collections.Generic;

namespace BridgeAlt.Models
{
    public class StatusResponse
    {
        public string status { get; set; }
    }
    public class FileDescription
    {
        public bool isFile { get; set; }
        public string name { get; set; }
        public long? size { get; set; }
    }
    public class DirectoryDescription
    {
        public string path { get; set; }
        public IEnumerable<FileDescription> files { get; set; }
    }
}