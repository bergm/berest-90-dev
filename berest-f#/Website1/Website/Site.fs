namespace Website

open IntelliFactory.Html
open IntelliFactory.WebSharper
open IntelliFactory.WebSharper.Sitelets

type Action =
    | Home
    | About

module Site =

    let Template title body : Content<Action> =
        PageContent <| fun context ->
            {
                Page.Default with
                    Title = Some title
                    Body = body context
            }

    let ( => ) text url =
        A [HRef url] -< [Text text]

    let Links (ctx: Context<Action>) =
        UL [
            LI ["Home" => ctx.Link Home]
            LI ["About" => ctx.Link About]
        ]

    let HomePage =
        Template "HomePage" <| fun ctx ->
            [
                Div [Text "HOME"]
                Links ctx 
                Div [new Controls.HelloControl()]
            ]

    let AboutPage =
        Template "AboutPage" <| fun ctx ->
            [
                Div [Text "ABOUT"]
                Links ctx
            ]

    let Main =
        Sitelet.Sum [
            Sitelet.Content "/" Home HomePage
            Sitelet.Content "/About" About AboutPage
        ]

type Website() =
    interface IWebsite<Action> with
        member this.Sitelet = Site.Main
        member this.Actions = [Home; About]

[<assembly: WebsiteAttribute(typeof<Website>)>]
do ()
