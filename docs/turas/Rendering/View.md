A view is a collection of a single camera and a [[Pipeline]] that can render the contents of all active scenes to an output image

A view should not be cached by a script, instead an aspect such as `OnRender(View& v)` will be provided to do per view logic 

