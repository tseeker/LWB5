// Text for AccessDenied
TechTrade.AccessDenied.title = 'Access denied';
TechTrade.AccessDenied.mainText = 'You are not authorized to use the alliance technology trading tool.';
TechTrade.AccessDenied.allianceTxt = 'Your current rank in your alliance does not have this privilege.';
TechTrade.AccessDenied.noAllianceTxt = 'You are not a member of any alliance.';
// Loading texts
TechTrade.Page.loadingText = "Loading technology list, please wait...";
TechTrade.Page.subLoadingText = "Loading page, please wait...";
// Text for the menu
TechTrade.Menu.text = [ 'Main page', 'Requests', 'Alliance list', 'Change orders', 'View orders' ];
// Main page text
TechTrade.Page.Main.submitTitle = "Technology list submission";
TechTrade.Page.Main.ordersTitle = "Technology trading orders";
TechTrade.Page.Main.lastSubText = "You last submitted your technology list at <b>__LS__</b>.";
TechTrade.Page.Main.nextSubText = "You will be able to submit your technology list again after <b>__NS__</b>.";
TechTrade.Page.Main.noLastSubText = "You have never submitted your technology list.";
TechTrade.Page.Main.submitButtonText = "Submit technology list";
TechTrade.Page.Main.noOrdersText = "No technology trading orders.";
TechTrade.Page.Main.sendOrderTitle = "Technology to send";
TechTrade.Page.Main.sendOrder = "We've been order to send the <a href='manual?p=tech___TI__'>__TN__</a> technology to player <a href='message?a=c&ct=0&id=__PI__'>__PN__</a>. The order was received at __OR__.";
TechTrade.Page.Main.sendOrderObeyed = "We obeyed this order at __OE__.";
TechTrade.Page.Main.sendButtonText = "Send technology";
TechTrade.Page.Main.recOrderTitle = "Technology to receive";
TechTrade.Page.Main.recOrder = "Player <a href='message?a=c&ct=0&id=__PI__'>__PN__</a> has been order to send us the <a href='manual?p=tech___TI__'>__TN__</a> technology. The order was issued at __OR__";
TechTrade.Page.Main.recOrderObeyed = " and obeyed at __OE__.";
// Requests page text
TechTrade.Page.Request.title = "Technology requests";
TechTrade.Page.Request.noReqText = "No requests at this time"
TechTrade.Page.Request.fiveRequests = "You already have 5 requests, unable to add more.";
TechTrade.Page.Request.noAvailableTechs = "There are no technologies you can request at this time.";
TechTrade.Page.Request.addRequest = "Add request for the following technology: ";
TechTrade.Page.Request.addRequestManual = "View manual page";
TechTrade.Page.Request.addRequestAdd = "Add";
TechTrade.Page.Request.noSelection = "-- Select technology --";
TechTrade.Page.Request.delRequest = "delete";
TechTrade.Page.Request.increasePriority = "up";
TechTrade.Page.Request.decreasePriority = "down";
TechTrade.Page.Request.submitText = "Submit requests";
// List page text
TechTrade.Page.List.playerHdr = 'Player';
TechTrade.Page.List.lastSubHdr = 'Last submission';
TechTrade.Page.List.vacationHdr = 'On vacation';
TechTrade.Page.List.restrainedHdr = 'Restrained';
TechTrade.Page.List.boolText = ['No', 'Yes'];
TechTrade.Page.List.techStatus = ['New', 'Foreseen', 'Implemented', 'Law'];
TechTrade.Page.List.requested = "Requested";
TechTrade.Page.List.playersPerPage = 'Players / page:';
TechTrade.Page.List.techsPerPage = 'Technologies / page:';
// Orders viewing page
TechTrade.Page.ViewOrders.title = 'Current technology trading orders';
TechTrade.Page.ViewOrders.noOrders = 'No technology trading orders have been issued yet.';
TechTrade.Page.ViewOrders.playerHdr = 'Player';
TechTrade.Page.ViewOrders.sendHdr = 'Send technology';
TechTrade.Page.ViewOrders.recvHdr = 'Receive technology';
TechTrade.Page.ViewOrders.noOrders = 'No orders';
TechTrade.Page.ViewOrders.sendTech = '<a href="manual?p=tech___TI__" target="_blank">__TN__</a> to <b>__PN__</b><br/>Order issued at __OI__.';
TechTrade.Page.ViewOrders.techSent = 'Sent at __OE__.';
TechTrade.Page.ViewOrders.receiveTech = '<a href="manual?p=tech___TI__" target="_blank">__TN__</a> from <b>__PN__</b><br/>Order issued at __OI__.';
TechTrade.Page.ViewOrders.techReceived = 'Received at __OE__.';
// Orders edition page
TechTrade.Page.SetOrders.title = 'Set alliance technology trading orders';
TechTrade.Page.SetOrders.alreadySubmitted = 'Technology trading orders were last submitted at __LS__.<br/>It will be possible to set new trading orders at __NS__.';
TechTrade.OrdersEditor.cantTrade = 'This player can\'t trade technologies';
TechTrade.OrdersEditor.sendTech = '<a href="manual?p=tech___TI__" target="_blank">__TN__</a> to <b>__PN__</b>.';
TechTrade.OrdersEditor.recvTech = '<a href="manual?p=tech___TI__" target="_blank">__TN__</a> from <b>__PN__</b>.';
TechTrade.OrdersEditor.addOrder = 'Set order';
TechTrade.OrdersEditor.delOrder = 'Remove order';
TechTrade.OrdersEditor.editOrder = 'Change order';
TechTrade.OrdersEditor.willReceive = '<b>__PN__</b> will receive __SP1__ from __SP2__';
TechTrade.OrdersEditor.willSend = '<b>__PN__</b> will send __SP1__ to __SP2__';
TechTrade.OrdersEditor.ok = 'Confirm';
TechTrade.OrdersEditor.cancel = 'Cancel';
TechTrade.OrdersEditor.selectPlayer = '--- select player ---';
TechTrade.OrdersEditor.selectTech = '--- select technology ---';
TechTrade.OrdersEditor.submitOrders = 'Submit orders';
TechTrade.OrdersEditor.confirmSubmit = 'You are about to submit new orders for the alliance\'s tech trading.\nYou will not be able to change these orders for the following 24h.\nPlease confirm.';
TechTrade.OrdersEditor.submitting = 'The orders are being sent, please wait ...';
TechTrade.OrdersEditor.submitError = 'An error occured while sending the technology trading orders.\nThis error can have different reasons:\n- someone else submitted orders,\n- a tech list has been updated and is no longer compatible with your orders.\n';
