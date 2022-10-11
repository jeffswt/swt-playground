/**
 * Make all your Pixiv following accounts private.
 *
 * Run this at https://www.pixiv.net/en/users/.../following
 */

async function sleep(duration) {
  return new Promise((resolve) => setTimeout(resolve, duration * 1000));
}

async function setFirstFollowingToPrivate() {
  // check page origin
  const rule = /^https:\/\/www.pixiv.net\/en\/users\/\d+\/following$/g;
  if (window.location.href.search(rule) < 0)
    throw new Error(`you must be at the 'following' page`);
  // define element signatures
  const itemListSig = "sc-1y4z60g-4 cqwgCG";
  const itemSig = "sc-1y4z60g-5 eUwTkI";
  const menuButtonSig = "sc-125tkm8-0 sc-125tkm8-3 ka-dhPl eZXKAK";
  const menuItemSig = "sc-1o6692m-0 bhnRuh";
  // find the element
  const itemList = document.getElementsByClassName(itemListSig);
  if (itemList.length !== 1) throw new Error(`cannot find unique item list`);
  const item = itemList[0].children[0];
  if (item === undefined) throw new Error(`cannot find item`);
  if (item.className !== itemSig) throw new Error(`mismatch item signature`);
  const menuButton = item.getElementsByClassName(menuButtonSig);
  if (menuButton.length !== 1) throw new Error(`cannot find menu button`);
  // extend menu
  await sleep(1.0);
  menuButton[0].children[0].click();
  // find element in menu
  await sleep(1.0);
  const menuItems = document.getElementsByClassName(menuItemSig);
  if (menuItems.length <= 0) throw new Error(`cannot find menu`);
  const menuItem = [...menuItems].filter((i) =>
    i.innerText.toLowerCase().includes("set as private")
  );
  if (menuItem.length !== 1)
    throw new Error(`cannot find 'set as private' button`);
  // click button!
  await sleep(1.0);
  menuItem[0].click();
  console.log("clicked button", menuItem);
  return;
}

async function privateAllPixivFollowings() {
  while (true) {
    await setFirstFollowingToPrivate();
    await sleep(4.0);
  }
}

void privateAllPixivFollowings();
