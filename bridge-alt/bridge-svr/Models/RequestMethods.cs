namespace BridgeAlt.Models
{
    public class AuthorizedRequest
    {
        public string token { get; set; }
    }
    public class LocationRequest : AuthorizedRequest
    {
        public string loc { get; set; }
    }
    public class VectorRequest : AuthorizedRequest
    {
        public string src { get; set; }
        public string dest { get; set; }
    }
}