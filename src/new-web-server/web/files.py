from jinja2 import Environment, FileSystemLoader, TemplateNotFound

class Files:
    def __init__(self, config: dict):
        self.jinja_dirs = config["jinja-dirs"]
        self.jinja_loader = FileSystemLoader(self.jinja_dirs, encoding="utf-8")
        self.jinja_env = Environment(loader=self.jinja_loader)

        self.static_dir = config["static-dir"]
        self.allowed_extensions = config["allowed-extensions"]
        self.pages = config["pages"]
        self.home_page = config["home-page"]
        self.encoding = config["encoding"]

    def getPage(self, page: str = "") -> str:
        if page == "":
            page = self.home_page
        try:
            return (
                bytes(self.jinja_env.get_template(page + ".html").render(), 
                      self.encoding), 
                      "text/html")
        except TemplateNotFound:
            return ("Template not found", 404)

    def getFile(self, file: str) -> bytes:
        (name, ext) = file.split(".") # name.ext
        content = ""
        content_type = ""

        if ext not in self.allowed_extensions:
            return ("Forbidden", 403)
        
        if ext in ["css", "js"]:
            path_to_file = self.static_dir + "/" + ext + "/" + file
            content_type = "text/css" if ext == "css" else "application/javascript"
        elif ext == "ico":
            path_to_file = self.static_dir + "/" + "assets/" + file
            content_type = "image/x-icon"
        elif ext == "json":
            path_to_file = self.static_dir + "/" + "assets/" + file
            content_type = "application/json"
        else:
            path_to_file = self.static_dir + "assets/" + file
            content_type = "image/" + ext
        try:
            with open(path_to_file, "rb") as f:
                content = f.read()
                return (content, content_type)
        except FileNotFoundError:
            return ("File not found", 404)
