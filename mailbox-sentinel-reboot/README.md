
# Mailbox Sentinel Reboot

Restarts your computer upon receiving some specific incoming email. The mail must be a message sent from yourself, into your own mailbox, with its subject and body matching certain keywords, in order to trigger the reboot.

A notification will be sent back to the very account after the daemon receives your request.

Log will be appended to a `rebootd.log` file at the working directory, which can be seen when the process is running in background mode (Tip: chances are that VSCode can act as a log viewer).

## Usage

First install Python 3.11 (3.8 should also work, but is not tested) and dependencies:

```sh
python -m pip install -r requirements.txt
```

Then you can perform a test run on the script:

```sh
python rebootd.py --subject "<mail_subject_keyword>" --body "<mail_body_keyword>"
```

## Configuring Autostart

To set this daemon to run at user login, simply:

1. Edit the arguments in `rebootd_shell.cmd` to fit your needs.
2. Create a shortcut to the VBScript `rebootd_launcher.vbs`.
3. Move the shortcut to your startup folder (`C:\Users\<your_username>\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup`).
4. The daemon should start automatically since next login. You may verify this by checking out the task manager.

## FAQ

- Q: Outlook cannot be spawned. What's going on?

  A: Either Outlook (O365 edition) is not installed, or you're using the beta version of Outlook. You have to be using the traditional edition to utilize the COM interface (which we depend on).

- Q: I'm using an IMAP email account, will it work?

  A: In theory it should, while it was initially designed to work for Exchange accounts.

- Q: I have multiple accounts. Will it work?

  A: It hasn't been tested *yet*, and we're welcome to PRs if it doesn't.
