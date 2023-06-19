(
var browser, webview;

webview = WebView()
    .minSize_(300@200);
browser = View(bounds:500@500).layout_(VLayout(
    webview
));

browser.front;

webview.url = "http://127.0.0.1:8080";
webview.onJavaScriptMsg = {|webview, msg| msg.postln; };
)

// [{"xPos":54.75576763737655,"yPos":51.655061680539205,"id":1,"connectionId":null},{"xPos":245.0779686658684,"yPos":146.28278892685947,"id":2,"connectionId":null},{"xPos":253.24268597336987,"yPos":348.47770676428587,"id":3,"connectionId":null},{"xPos":53.07606313728229,"yPos":143.89727492836226,"id":4,"connectionId":null},{"xPos":246.75543766072693,"yPos":245.45212436902966,"id":5,"connectionId":null},{"xPos":352.2118540446158,"yPos":245.40447355015323,"id":6,"connectionId":null},{"xPos":48.1476228662839,"yPos":254.15512518139144,"id":7,"connectionId":null},{"xPos":498.1189002339156,"yPos":485.10584896001706,"id":8,"connectionId":null},{"xPos":159.74350423363597,"yPos":149.8662234920878,"id":9,"connectionId":null},{"xPos":151.22703361702054,"yPos":350.97108362653324,"id":10,"connectionId":null},{"xPos":152.51641019252327,"yPos":248.8479830360805,"id":11,"connectionId":null},{"xPos":47.599936503409765,"yPos":351.5495863878125,"id":12,"connectionId":null},{"xPos":154.65128349219827,"yPos":48.562659984370015,"id":13,"connectionId":null},{"xPos":246.22675525947545,"yPos":53.66074669214572,"id":14,"connectionId":null},{"xPos":354.47872709916845,"yPos":147.76728686557084,"id":15,"connectionId":null}]
