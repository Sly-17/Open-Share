# Open-Share
Share content (markup or plain-text) over HTTP


Usage : ```share [OPTION] [FILE]```                                                                                                                              
share is a tool that allows serving content (markup or plain text) over HTTP.                                                                              
                                                                                                                                                           
**SERVING BY PASTING CONTENT:**                                                                                                                              
Example - To share some content over HTTP at PORT 4993, run : ```share 4993```                                                                                   
Paste your content and hit `<enter>` followed by `<ctrl c>` to start serving.                                                                                  
The serving stays live on PORT 4993 until you kill the process.                                                                                            
                                                                                                                                                           
**SERVING A FILE:**                                                                                                                                            
Example - To server a file say ../data/data.txt over HTTP at PORT 3829, run : ```share 3829 ../data/data.txt```                                                
The serving stays live until you kill the process.
