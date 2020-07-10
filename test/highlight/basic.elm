module Main exposing (Msg(..), main, update, view)

import Browser
// ^ keyword
import Html exposing (Html, button, div, text)
// ^ keyword
import Html.Events exposing (onClick)
// ^ keyword


main =
    Browser.sandbox { init = 0, update = update, view = view }


type Msg
    = Increment
    | Decrement


update msg model =
    case msg of
        Increment ->
            model + 1

        Decrement ->
            model - 1


view model =
    div []
        [ button [ onClick Decrement ] [ text "-" ]
        , div [] [ text (String.fromInt model) ]
        , button [ onClick Increment ] [ text "+" ]
        ]
