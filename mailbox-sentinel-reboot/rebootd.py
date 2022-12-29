
import argparse
import ctypes
from dataclasses import dataclass
import datetime
import enum
import os
import time
from typing import List
import win32com.client


@dataclass
class GlobalConfig:
    reboot_user_email: str
    reboot_subject_magic: str
    reboot_body_magic: str
    # messages that are too old will be ignored
    reboot_message_delay: float
    # send notification to target email
    notification_email_to: str
    # set this to false to stop daemon
    main_thread_running: bool
    # each loop takes around this many seconds
    loop_duration: float
    # will wait this many seconds until reboot
    reboot_delay: float


global_config = GlobalConfig(
    reboot_user_email='<auto_filled>',
    reboot_subject_magic='<command_line_args>',
    reboot_body_magic='<command_line_args>',
    reboot_message_delay=300.0,  # 5 mins
    notification_email_to='<auto_filled>',
    main_thread_running=True,
    loop_duration=60,  # 1 min
    reboot_delay=60,  # 1 min
)


@dataclass
class Email:
    received_at: datetime.datetime
    sent_from: str
    subject: str
    body: str


class OlDefaultFolders(enum.Enum):
    # https://learn.microsoft.com/en-us/office/vba/api/outlook.oldefaultfolders
    olFolderCalendar = 9
    olFolderConflicts = 19
    olFolderContacts = 10
    olFolderDeletedItems = 3
    olFolderDrafts = 16
    olFolderInbox = 6
    olFolderJournal = 11
    olFolderJunk = 23
    olFolderLocalFailures = 21
    olFolderManagedEmail = 29
    olFolderNotes = 12
    olFolderOutbox = 4
    olFolderSentMail = 5
    olFolderServerFailures = 22
    olFolderSuggestedContacts = 30
    olFolderSyncIssues = 20
    olFolderTasks = 13
    olFolderToDo = 28
    olPublicFoldersAllPublicFolders = 18
    olFolderRssFeeds = 25


def pick_emails(driver, predicate, set_read=False) -> List[Email]:
    global global_config

    namespace = driver.GetNameSpace('MAPI')
    folder = namespace.GetDefaultFolder(OlDefaultFolders.olFolderInbox.value)
    items = folder.Items
    result: List[Email] = []

    message = items.GetLast()
    while message:
        # skip read mails
        if not message.UnRead:
            message = items.GetPrevious()
            continue
        # convert mail
        mail = Email(
            sent_from=message.Sender.Address,
            received_at=datetime.datetime.strptime(str(message.ReceivedTime), '%Y-%m-%d %H:%M:%S.%f%z').replace(tzinfo=None),
            subject=message.Subject,
            body=message.Body,
        )
        # we've gone too far.
        delta_t = (datetime.datetime.now() - mail.received_at).total_seconds()
        if delta_t > global_config.reboot_message_delay:
            break
        # skip mails not marked as 'should pick'
        if not predicate(mail):
            message = items.GetPrevious()
            continue
        # save & mark as read
        result.append(mail)
        if set_read:
            message.UnRead = False
        message = items.GetPrevious()

    # done
    return result


def send_notification(driver) -> None:
    global global_config
    sleep_dur = global_config.reboot_delay

    message = driver.CreateItem(0)
    message.To = global_config.notification_email_to
    message.Subject = "[Auto-reply] Restarting workstation"
    message.Body = f"Received command; your workstation will be restarted in {sleep_dur} seconds."
    message.Send()
    return


def force_restart() -> None:
    global global_config
    duration = int(global_config.reboot_delay)
    os.system(f'shutdown /r /f /t {duration}')
    return


def send_desktop_notification() -> None:
    global global_config
    ctypes.windll.user32.MessageBoxW(
        0,
        "Enter 'shutdown /a' in 'cmd' to interrupt restart.",
        f"Workstation restarting in {global_config.reboot_delay}s",
        1,
    )
    return


def watchdog_service() -> None:
    global global_config

    # start outlook driver
    driver = win32com.client.Dispatch('outlook.application')
    namespace = driver.GetNameSpace('MAPI')
    mail_address = namespace.CurrentUser.Address
    smtp_address = namespace.CurrentUser.AddressEntry.GetExchangeUser().PrimarySmtpAddress
    global_config.reboot_user_email = mail_address
    global_config.notification_email_to = smtp_address
    log(f'resolved exchange account <{mail_address}>')
    log(f'resolved email <{smtp_address}>')
    log(f'mail subject keyword: "{global_config.reboot_subject_magic}"')
    log(f'mail body keyword: "{global_config.reboot_body_magic}"')

    # infinite loop
    def picker(mail: Email) -> bool:
        if mail.sent_from != global_config.reboot_user_email:
            return False
        now = datetime.datetime.now()
        delta = datetime.datetime.now() - mail.received_at
        delta_f = delta.total_seconds()
        if delta_f > global_config.reboot_message_delay:
            return False
        if global_config.reboot_subject_magic not in mail.subject:
            return False
        if global_config.reboot_body_magic not in mail.body:
            return False
        return True

    while True:
        log('reading mailbox...')
        mails = pick_emails(driver, picker, set_read=True)
        # notify users about the decision
        if not mails:
            log(f'no command received.')
            time.sleep(global_config.loop_duration)
            continue
        log(f'received {len(mails)} commands, waiting to reboot')
        log(f'sent reboot notification to <{global_config.notification_email_to}>.')
        send_notification(driver)
        log(f'will reboot in {global_config.reboot_delay} seconds.')
        force_restart()
        log(f'reboot triggered.')
        send_desktop_notification()
        break
    return


def log(message: str) -> None:
    print(f'[{datetime.datetime.now()}] {message}')


if __name__ == '__main__':
    # define arguments
    parser = argparse.ArgumentParser(
        prog = 'RebootWatcher',
        description = 'Watches your Outlook Inbox and restarts computer.')
    parser.add_argument(
        '--subject', dest='subject', action='store', required=True, type=str,
        help='Find this keyword in the sentinel message title.')
    parser.add_argument(
        '--body', dest='body', action='store', required=True, type=str,
        help='Find this keyword in the sentinel message body.')
    args = parser.parse_args()

    # extract arguments
    global_config.reboot_subject_magic = args.subject
    global_config.reboot_body_magic = args.body
    watchdog_service()
