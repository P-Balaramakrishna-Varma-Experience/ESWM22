- first time setup https://flask.palletsprojects.com/en/2.2.x/installation/

## Running Basics
- flask --app hello run 
- If you have the debugger disabled or trust the users on your network, you can make the server publicly available simply by adding --host=0.0.0.0 to the command line:
- select port to run --port
- debug mode for autoreload of code, trace, iteractive shell.

##
-  any user-provided values rendered in the output must be escaped to protect from injection attacks (security)
- Use the route() decorator to bind a function to a URL. 
  - the function return value taken as html (default)
  - dynamic url the dynamic part is data passed to the function
  - given functionname derive the path.. usefull less hardcoding (url_for)
- Http Methods
    - @app.get("url") @app.pots("url")
    - record = json.loads(request.data) {post data}