
# No Postman Cloud

If you've accidentally leaked your corporate API keys into Postman, and have Postman uploaded them to its own cloud services, and corporate policy requires that you shall rotate them...

Use this to dump all your Postman history and use other tools to change your keys.

## Usage

Note that since some of the dependencies require building wheels from source, you may consider using a WSL environment (or just install VC++ build tools or gcc toolchain). Use a pre-built binary if you find it troublesome to install on either of the platforms.

```bash
python3 postmanout.py --output /path/to/output.jsonl  # deps are automatically installed
```

The output can be a JSON file of an array or a JSONL of objects, each object follows:

```json
{
  "kind": "history",
  "timestamp": "1970-01-02T22:33:44.567000Z",
  "method": "GET",
  "url": "https://example.com/api/v1/endpoint",
  "headers": {
    "Authorization": "Bearer {{token}}",
    "User-Agent": "PostmanRuntime/7.28.4"
  },
  "collection": "11111111-2222-3333-4444-555555555555"
}
```

We also provide global variables and collection variables in the output, so that one could easily track prior values of variables. Note that since we couldn't find a way to track the entire history (and timestamp) of variable changes, and that network requests are not permanently logged either, we only track the last value of each variable.

```json
{
  "kind": "collection",
  "id": "11111111-2222-3333-4444-555555555555",
  "name": "My Private Collection (GET)",
  "values": {
    "token": [
      "eyJ...eyJ"
    ],
    "some_user_name": [
      "this_user",
      "that_user"
    ]
  }
}
```

There are other fields such as `body` which are not covered by this script, which are commented in the source files and can be easily retrieved.

## Build

```powershell
conda create -y -n postmanout python=3.10
conda activate postmanout
python -m pip install pyinstaller
python ./postmanout.py
$pythonPath = $(Get-Command python).Source
$snappyPath = "$(Resolve-Path "$pythonPath/../Lib/site-packages/python_snappy.libs/")"
pyinstaller --onefile postmanout.py --add-data "${snappyPath}:./"
```
