
# Mailbox Sentinel Reboot

Restarts your computer upon receiving some specific incoming email. The mail must be a message sent from yourself, into your own mailbox, with its subject and body matching certain keywords, in order to trigger the reboot.

A notification will be sent back to the very account after the daemon receives your request.

## Usage

First install Python 3.11 (3.8 should also work) and dependencies:

```sh
python -m pip install -r requirements.txt
```

Then you can perform a test run on the script:

```sh
python rebootd.py --subject '<mail_subject_keyword>' --body '<mail_body_keyword>'
```

## FAQ

- Q: Outlook cannot be spawned. What's going on?

  A: Either Outlook (O365 edition) is not installed, or you're using the beta version of Outlook. You have to be using the traditional edition to utilize the COM interface (which we depend on).

- Q: I'm using an IMAP email account, will it work?

  A: In theory it should, while it was initially designed to work for Exchange accounts.

- Q: I have multiple accounts. Will it work?

  A: It's not been tested *yet*, and we're welcome to PRs if it doesn't.
