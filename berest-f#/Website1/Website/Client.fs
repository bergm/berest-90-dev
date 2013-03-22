namespace Website

module Client =
    open IntelliFactory.WebSharper
    open IntelliFactory.WebSharper.Html

    [<JavaScript>]
    let SayHello () =
        JavaScript.Alert("HELLO!")

    [<JavaScript>]
    let MakeHelloDiv () =
        SayHello ()
        Div [ Text "HELLO THERE" ]
    
module Controls =
    open IntelliFactory.WebSharper

    type HelloControl() =
        inherit Web.Control()

        [<JavaScript>]
        override this.Body = Client.MakeHelloDiv() :> _
        
