# Configuration Class

IND = "  " # indent

base_class = """class CfgSection {
 public:
  CfgSection(data::Streams &streams): streams_(streams){}
  
 protected:
  mutable std::mutex cfg_lock_ = std::mutex();
  data::Streams &streams_;
};\n
"""

def getConfigurationClass(initializers, private_members):
    init_part = ""
    
    for initializer in initializers:
        init_part += f'{IND*2}{initializer}(streams),'
        
    members = ""
    for p in private_members:
        members += f'{IND}{p}\n'
        
    return """class Configuration {{
 public:
  Configuration(data::Streams &streams):
{0}
    streams_(streams){{}}

{1} 
 private:
  data::Streams &streams_;
}};\n
""".format(init_part, members)
