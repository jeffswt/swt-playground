// Batch cancel all upvotes from your user profile page.
// Hard-limited to several hundred transactions per day until rate limit
// exceeded (HTTP 429 Error).

async function cancelUpvotes() {
  function sleep(duration) {
    return new Promise((resolve) => setTimeout(resolve, duration * 1000));
  }

  while (true) {
    // pick 1 upvoted button until there's nothing to pick
    let buttons = document.getElementsByClassName(
      "Button VoteButton VoteButton--up is-active"
    );
    if (buttons.length == 0) break;
    let button = buttons[0];
    // click and log and wait
    button.click();
    console.log(button);
    await sleep(1.0);
  }
  console.log("Cancelled all (visible) upvotes");
}
cancelUpvotes();
