/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename MapApplet.java
*
* @purpose Code in support of the MapApplet.html page
*
* @component unitmgr
*
* @comments This applet can only be viewed in Internet Explorer 5+
*
* @create 09/29/2000
*
* @author jjernigan, jlshaw
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.lang.*;
import java.net.*;
import java.util.*;

import javax.swing.*;

import netscape.javascript.*;

public class MapApplet extends Applet implements ActionListener, MouseListener, Runnable
{
  private GridLayout gridlayout;    // layout type

  private int displayedUnit;

  private int isStacking;
  private int stackable;
  private int toggleSupported;
  private int isXUI;

  private Image portImage;          // image of an some empty port
  private Image background;         // image of the switch
  private Image slotimage;          // image of the populated slot faceplate
  private Image linkUpImage;        // linkup image for some port

  private Image navImagePrev;          // image for prev unit navigation
  private Image navImageNext;          // image for next unit navigation
  private Image navImageBlank;         // image for navigation


  private Image stackLinkImage;          // image for stack applet link

  private String actions[];         // stores urls for menuitems
  private String targets[];         // stores target frame to place the web page specified by actions[]
  private int menuItemCtr;          // counter for all menuitems in all menus
  private MenuItem menuItems[];     // array of MenuItems
  private Menu menus[];             // array of Menus
  private Menu portMenu;            // the Menu for port PopupMenu
  private PopupMenu portPopupMenu;  // the PopupMenu for ports
  private PopupMenu fpsIntfPopupMenu;  // the PopupMenu for FPS Interfaces 
  private PopupMenu stackIntfPopupMenu;  // the PopupMenu dedicated stack interfaces
  private Menu switchMenu;          // the Menu for switch PopupMenu
  private PopupMenu switchPopupMenu;// the PopupMenu for switch

  private Vector allPorts = new Vector();// for getting port info
  private JSObject win;             // object that allows javascript calls and evals

  private int slotimagepresent;     // indicates whether the slot image should be used
  private int L7_MAX_MENUITEMS;                  // maximum number of total menu items

  private final static int USMWEB_OFFSET_SERVICE_PORT  = 2;   // TRGTRG to be removed 
  private final static int USMWEB_OFFSET_SERIAL_PORT   = 3;   // TRGTRG to be removed 
  private final static int USMWEB_OFFSET_STATUS_LED    = 4;   // TRGTRG to be removed 
  private final static int USMWEB_OFFSET_FANSTATUS_LED = 5;   // TRGTRG to be removed 
  private final static int USMWEB_OFFSET_SFP           = 6;   // TRGTRG to be removed 


  private final static int INTF_PHYSICAL  = 1;   
  private final static int INTF_SFP       = 2;   
  private final static int INTF_STACK     = 3;   
  private final static int INTF_SERIAL    = 4;   
  private final static int INTF_SERVICE   = 5;   


  private String company;         // company prefix

  private final static int FRONT_PANEL   = 1;
  private final static int BACK_PANEL    = 2;
  private final static int DYNAMIC_PANEL = 3;
  private final static int END_OF_DATA   = 0x11223344;

  private final static int APPLET_IFLAG_STACKING_MASK      = 0x0000FF00;    
  private final static int APPLET_IFLAG_FPS_SUPPORT        = 0x00000100;    
  private final static int APPLET_IFLAG_FPS_MODE_SUPPORT   = 0x00000200;
  private final static int APPLET_IFLAG_FPS_MODE           = 0x00000400;
  private final static int APPLET_IFLAG_FPS_STACK_MODE     = 0x00000400;
  private final static int APPLET_IFLAG_STACK_MODE_ONLY    = 0x00000800;
  private final static int APPLET_IFLAG_STACK_MODE_SUPPORT = 0x00001000;
  private final static int APPLET_IFLAG_STACK_MODE_HIGIG   = 0x00002000;
  private final static int APPLET_IFLAG_STACK_MODE_10G     = 0x00004000;
  private final static int APPLET_IFLAG_POE_MODE           = 0x00008000;

  private Stack stack; 
  private boolean update_in_progress = true;
  private boolean toggle_in_progress = false;
  private boolean clickPaint = false;
  private boolean stackFullView = false;
  private boolean validConnection = false;

  private Rectangle stackLink;

  private boolean DEBUG = false;
  private final static int MODE_DEBUG = 0x00000004;    
  private boolean VERBOSE = false;
  private final static int MODE_VERBOSE = 0x00000002;    
  private boolean READ_DATA_TRACE = false;
  private final static int MODE_READ_DATA_TRACE = 0x00000001;    


  /* socket thread vars */
  private int ledFanStatus;                       // fan status LED state
  private int ledStatus;                          // status LED state
  private String host = new String();             // ip of switch
  private Thread updateThread = null;             // thread for socket
  private DataInputStream dis;                    
  private DataOutputStream dos;
  private Socket s;

  private int NUMBYTES;                                // indicates the length of the array to send through the socket
  private final int sleepTime      = 1000;             // time to wait between checks on the port status, in milliseconds
  private final int retrySleepTime = 10000;            // time to wait after failure before retries


  // ---------------------------------------------------------------------------
  //  These items need to be known on both sides of the socket
  // ---------------------------------------------------------------------------

  private final int WJS_OFF   = 2;                    // tells the socket to close on the other side
  private final int APPLET_CODE   = 8;                // tells the socket what action to perform on the data
  private final int HOST_PORT     = 4242;             // port to travel through

  private int L7_MAX_SUPPORTED_STACK_UNITS  = 1; 

  private final static int INTF_TYPE_DATA_RJ45    = 101;         // TRGTRG ... should I dump these into the html so they are defined once??
  private final static int INTF_TYPE_DATA_GBIC    = 102;
  private final static int INTF_TYPE_DATA_SFP     = 103;
  private final static int INTF_TYPE_DATA_SCDUP   = 104;
  private final static int INTF_TYPE_DATA_XAUI    = 105;
  private final static int INTF_TYPE_SERIAL_PORT  = 201;
  private final static int INTF_TYPE_SERVICE_PORT = 301;
  private final static int INTF_TYPE_STACKING     = 401;

  private final static int USMWEB_LED_STATUS_GREEN  = 1;      // These must agree with the values in web_java.h 
  private final static int USMWEB_LED_STATUS_AMBER  = 2;
  private final static int USMWEB_LED_STATUS_YELLOW = 3;
  private final static int USMWEB_LED_STATUS_RED    = 4;
  private final static int USMWEB_LED_STATUS_OFF    = 5;


  private final static int OPTION_TYPE_TOGGLE = 101;

  private int readData(String info)
  {
    int dataInt;
    try
    {
      dataInt = dis.readInt();
      NUMBYTES++;
    }
    catch (Throwable rdt)
    {
      // TRGTRG - should we trap this and take further action - return(-1) ???? 
      System.out.println("readData() : Error reading input Data for "+info);
      dataInt = -1;
    }
    if (READ_DATA_TRACE) System.out.println(info+" : "+dataInt+"   (0x"+Long.toHexString(dataInt).toUpperCase()+")");
    if (dataInt == END_OF_DATA) 
    {
      if (READ_DATA_TRACE) System.out.println(NUMBYTES+" data reads");
      NUMBYTES = 0;
    }
    return dataInt;
  }



  private String stringParameterGet(String parmStr)
  {
    String strVal = null;

    try
    {
      strVal = new String(getParameter(parmStr));
    }
    catch (Throwable T)
    {
      System.out.println("Warning - failed to parse string parameter "+parmStr+".");
      strVal = null;
    }

    return strVal;
  }

  private int integerParameterGet(String parmStr, int defaultVal)
  {
    int    val = 0;
    String strVal = null;

    strVal = stringParameterGet(parmStr);
    try
    {
      val = Integer.parseInt(strVal);
    }
    catch (Throwable T)
    {
      System.out.println("Warning - failed to parse integer from the string "+strVal+".  Value will be set to "+defaultVal+".");
      val = defaultVal;
    }
    return val;
  }

  private Image imageGet(String parmStr)
  {
    Image   jImage = null;
    String  imageName; 

    imageName = stringParameterGet(parmStr);
    if (imageName == null)
    {
      System.out.println("Warning - failed to parse image parameter "+parmStr+".  Image will be set to null.");
      jImage = null;
    }
    else
    {
      try
      {
        jImage = getImage(getCodeBase(), imageName); 
      }
      catch (Throwable T)
      {
        System.out.println("Warning - failed to retrieve image "+imageName+".  Image will be set to null.");
        jImage = null;
      }
    }

    return jImage;
  }

  private Dimension dimensionGet(String parmStr)
  {
    Dimension dim;
    String  dimName;
    StringTokenizer st;

    dimName = stringParameterGet(parmStr);
    if (dimName == null)
    {
      System.out.println("Warning - failed to parse dimension parameter "+parmStr+".  Dimension will be set to (0,0).");
      dim = new Dimension(0,0);
    }
    else
    {
      st= new StringTokenizer(dimName,",()"); 
      try 
      {
        dim = new Dimension(Integer.parseInt(st.nextToken()), Integer.parseInt(st.nextToken()));
      }
      catch (Throwable T)
      {
        System.out.println("Warning - failure tokenizing dimension string "+dimName+".  Dimension will be set to (0,0).");
        dim = new Dimension(0,0);
      }
    }
    return dim;
  }

  private Point locationGet(String parmStr)
  {
    Point loc;
    String  locName;
    StringTokenizer st;

    locName = stringParameterGet(parmStr);
    if (locName == null)
    {
      System.out.println("Warning - failed to parse location parameter "+parmStr+".  Location will be set to (0,0).");
      loc = new Point(0,0);
    }
    else
    {
      st= new StringTokenizer(locName,",()"); 
      try 
      {
        loc = new Point((Integer.parseInt(st.nextToken())), (Integer.parseInt(st.nextToken())));
      }
      catch (Throwable T)
      {
        System.out.println("Warning - failure tokenizing location string "+locName+".  Location will be set to (0,0).");
        loc = new Point(0,0);
      }
    }
    return loc;
  }

  private int xLocGet(String parmStr)
  {
    int xloc;
    String  locName;
    StringTokenizer st;

    locName = stringParameterGet(parmStr);
    if (locName == null)
    {
      System.out.println("Warning - failed to parse x-location parameter "+parmStr+". X will be set to 0.");
      xloc = 0;
    }
    else
    {
      st= new StringTokenizer(locName,",()"); 
      try 
      {
        xloc = Integer.parseInt(st.nextToken());
      }
      catch (Throwable T)
      {
        System.out.println("Warning - failure tokenizing location string "+locName+".  X will be set to 0.");
        xloc = 0;
      }
    }
    return xloc;
  }

  private int yLocGet(String parmStr)
  {
    int yloc;
    String  locName;
    StringTokenizer st;

    locName = stringParameterGet(parmStr);
    if (locName == null)
    {
      System.out.println("Warning - failed to parse x-location parameter "+parmStr+". Y will be set to 0.");
      yloc = 0;
    }
    else
    {
      st= new StringTokenizer(locName,",()"); 
      try 
      {
        Integer.parseInt(st.nextToken());
        yloc = Integer.parseInt(st.nextToken());
      }
      catch (Throwable T)
      {
        System.out.println("Warning - failure tokenizing location string "+locName+".  Y will be set to 0.");
        yloc = 0;
      }
    }
    return yloc;
  }


  private int connectorfTypeGet(String intfStr)
  {
    int intf_type = INTF_TYPE_DATA_RJ45;

    if (intfStr == null)
    {
      System.out.println("Warning - failed to interpret the interface type (null string).  Value will be set to INTF_TYPE_DATA_RJ45.");
    }
    else
    {
      if (intfStr.compareTo("rj45") == 0)
      {
        intf_type = INTF_TYPE_DATA_RJ45;  
      }
      else if (intfStr.compareTo("scdup") == 0)
      {
        intf_type = INTF_TYPE_DATA_SCDUP; 
      }
      else if (intfStr.compareTo("xaui") == 0)
      {
        intf_type = INTF_TYPE_DATA_XAUI;  
      }
      else
      {
        System.out.println("Warning - failed to interpret the interface type from the string "+intfStr+".  Value will be set to INTF_TYPE_DATA_RJ45.");
      }
    }

    return intf_type;
  }



  /*********************************************************************
  *
  * @functions init
  *
  * @purpose Initializes the applet, variables, store port info
  *          
  * @param none
  *
  * @returns none
  *
  * @notes   Automatically called when the applet is loaded.
  *
  * @end
  *
  *********************************************************************/

  public void init()
  {
    Dimension       dim;
    Point           loc;
    StringTokenizer st2;

    displayedUnit = 0;
    NUMBYTES = 0;

    L7_MAX_MENUITEMS = integerParameterGet("L7_MAX_MENUITEMS", 512);
    stackable = integerParameterGet("STACKABLE", 0);

    if (stackable == 1)
    {
      L7_MAX_SUPPORTED_STACK_UNITS = integerParameterGet("STACK_MAX_NUM_UNITS", 1);
      if (integerParameterGet("STACK_FULL_VIEW", 0) == 1)
        stackFullView = true;
      else
        stackFullView = false;
    }
    else
    {
      L7_MAX_SUPPORTED_STACK_UNITS = 1;
      stackFullView = false;
    }

    company = stringParameterGet("company");


    /*  gets the ip address using the codebase url
              "http://0.0.0.0/java"   */
    host=getCodeBase().toString();
    int slashIndex= host.indexOf("/");

    while (slashIndex < 7)
    {
      host=host.substring(slashIndex+1);
      slashIndex= host.indexOf("/");
    }
    host=host.substring(0, slashIndex);

    /*  Checking if a port number has been specified for connection to the 
        emweb server (like it may be for a non-default server-side port setting 
        for SSL). In this case, we remove the ":xxx" port part of the host 
        IP address string
    */
    slashIndex = host.indexOf("[");
    if(slashIndex != -1)
    {
       host=host.substring(slashIndex+1);
    }

    int colonIndex = host.indexOf("]");
    if (colonIndex != -1)
    {
      host=host.substring(0, colonIndex);
    }
    gridlayout=new GridLayout();

    /* These arrays store all the menuItems, their actions and their targets.
       Since there are two different menus, these had to be initialized outside
       so that the information would not be overwritten by the parseData() function
       on the second call.  So there is a max number of menuItems specified in the
       initialization.*/
    actions = new String[L7_MAX_MENUITEMS];
    targets = new String[L7_MAX_MENUITEMS];
    menuItems = new MenuItem[L7_MAX_MENUITEMS];
    menuItemCtr = -1;

    String dataPortMenu = stringParameterGet("IntfMenu");
    portPopupMenu       = parseData(dataPortMenu);

    String dataSwitchMenu = stringParameterGet("FullMenu");
    switchPopupMenu       = parseData(dataSwitchMenu);

    if (stackable == 1)
    {
      String datafpsIntfMenu = stringParameterGet("fpsIntfMenu");
      fpsIntfPopupMenu       = parseData(datafpsIntfMenu);
      String dataStackIntfMenu = stringParameterGet("stackIntfMenu");
      stackIntfPopupMenu       = parseData(dataStackIntfMenu);

      loc = locationGet("full_stack_link");
      dim = dimensionGet("full_stack_link_dim");
      stackLink = new Rectangle(loc, dim);
      stackLinkImage = imageGet("full_stack_link_img");  

      navImagePrev = imageGet("stack_nav_prev"); 
      navImageNext = imageGet("stack_nav_next");
      navImageBlank = imageGet("stack_nav_blank");
    }


    background=null;   // Stack background not currently used
    slotimagepresent = 0;

    // TRGTRG need to read these items from the html
    stack = new Stack (L7_MAX_SUPPORTED_STACK_UNITS, 1, true, background);


    URL url = null;  
    InputStream stream = null;  

    this.addMouseListener(this);
    this.setLayout(gridlayout);
    this.add(portPopupMenu);
    this.add(switchPopupMenu);

    if (updateThread == null)
    {
      update_in_progress = true;  // Don't paint until the first update completes
      updateThread = new Thread(this, "portLinkStatus");
      updateThread.start();
    }
  }  // end init


  /*********************************************************************
  *
  * @functions destroy
  *
  * @purpose Stop the thread and close the socket.
  *          
  * @param none
  *
  * @returns none
  *
  * @notes   This function is automatically called when.
  *
  * @end
  *
  *********************************************************************/
  public void destroy() 
  {
    updateThread = null;
    /*  updateThread.destroy(); */
    try
    {
      dos.flush();
      dos = new DataOutputStream (new BufferedOutputStream(s.getOutputStream()));
      dos.writeInt(WJS_OFF);            
      dos.flush();
      this.showStatus("Disconnecting from " + host + " on TCP port " + HOST_PORT );
      s.close();
    }
    catch (Throwable t)
    {
    }
  }

  /*********************************************************************
  *
  * @functions run
  *
  * @purpose Connect to socket, initialize port status array,
  *          and continuously checking for changes in port link state.
  *          
  * @param none
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void run()
  {

    Thread myThread = Thread.currentThread();
    int updateFlag = 0;
    int numFailures = 0;

    try
    {
      /***** connect to host ********/
      s = new Socket(host, HOST_PORT);
      this.showStatus("Socket connected to " + host + " on TCP port " + HOST_PORT );
      dis = new DataInputStream(s.getInputStream());
      dos = new DataOutputStream (new BufferedOutputStream(s.getOutputStream()));
      validConnection = true;

      while (updateThread == myThread)
      {
        try
        {
          updateFlag = updateVectorStates();
        }
        catch (Throwable trg1)
        {
          System.out.println("updateVectorStates() failure" );
          updateFlag = -1;
          dos.flush();
        }
        if (updateFlag != -1)
        {
          if ((updateFlag == 1) || (clickPaint == true)) /* update the vector states.  if it changed, repaint */
          {
            try
            {
              validate();
            }
            catch (Throwable trg2)
            {
              System.out.println("validate() failure" );
            }
            try
            {
              repaint();
            }
            catch (Throwable trg3)
            {
              System.out.println("repaint() failure" );
            }
            clickPaint = false;
          }
        }
        else
        {
            System.out.println("re-initializing the applet" );

            try
            {
              if(dos != null) 
                dos.flush();
            }
            catch (Throwable retryConnect)
            {
              dos = null;
              continue;
            }

            try 
            {
              if(s != null)
                s.close();
            }
            catch (Throwable retryConnect)
            {
              s = null;
              continue;
            }

            Thread.sleep(retrySleepTime);  
            try 
            {
              s = new Socket(host, HOST_PORT);
              dis = new DataInputStream(s.getInputStream());
              dos = new DataOutputStream (new BufferedOutputStream(s.getOutputStream()));
              validConnection = true;
            }
            catch (Throwable retryConnect)
            {
              System.out.println("failed to re-establish the applet connection" );
              validConnection = false;
              continue;
            }
        }
        try
        {
          Thread.sleep(sleepTime);                     
        }
        catch (Throwable t1)
        {
          try
          {
            dos = new DataOutputStream (new BufferedOutputStream(s.getOutputStream()));
            dos.writeInt(WJS_OFF);                  
            dos.flush();
            System.out.println("Disconnecting from "+host+" on TCP port "+HOST_PORT+" Thead.sleep()" );
            s.close();
            validConnection = false;
          }
          catch (Throwable t2)
          {
            // if (DEBUG) System.out.println("Unknown failure in java applet t2");
          }
        }
      }
      if (updateThread != myThread)
      {
        // if (DEBUG) System.out.println("updateThread != myThread" );
      }
    }
    catch (Throwable t3)
    {
      try
      {
        dos = new DataOutputStream (new BufferedOutputStream(s.getOutputStream()));
        dos.writeInt(WJS_OFF);            
        dos.flush();
        System.out.println("Disconnecting from " + host + " on TCP port " + HOST_PORT );
        s.close();
      }
      catch (Throwable t4)
      {
        // if (DEBUG) System.out.println("Unknown failure in java applet t4");
      }
    }

  }  // end run

  /*********************************************************************
  *
  * @functions remove cards from unit structure and call remove ports
  *
  * @purpose 
  *             
  * @param curr_unit 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  public void removeSlotPorts(Unit curr_unit)
  {
    int c, s, p, first;
    int curr_panel;
    Card curr_card;
    Interface intfObj;


    // if (DEBUG) System.out.println("removeSlotPorts for unit "+curr_unit.getUnitID()+" called");

    curr_panel = curr_unit.getCurrentPanel();

    for (c=0; c<curr_unit.getNumCards(); c++)
    {
      curr_card = curr_unit.getCard(c);
      if ((curr_card != null) && (curr_panel == curr_card.getPanelLocation()))
      {
        // if (VERBOSE) System.out.println("removing ports for card"+c);
        removePorts(curr_card);
        // if (VERBOSE) System.out.println("removing card"+c);
        curr_unit.removeCard(c);
      }
    }

    first = curr_unit.getFirstStackIntfNum();
    for (p=first; p<(curr_unit.getNumStackIntf()+first); p++)
    {
      intfObj = curr_unit.getStackIntf(p);
      if ((intfObj != null) && (curr_panel == intfObj.getPanelLocation()))
      {
        // if (VERBOSE) System.out.println("removing HiGig"+p);
        remove (intfObj);
        curr_unit.removeStackIntf(p);  
      }
    }

    intfObj = curr_unit.getServicePort();
    if ((intfObj != null) && (curr_panel == intfObj.getPanelLocation()))
    {
      // if (VERBOSE) System.out.println("removing servicePort");
      remove (intfObj);
      curr_unit.removeServicePort();
    }

    intfObj = curr_unit.getSerialPort();
    if ((intfObj != null) && (curr_panel == intfObj.getPanelLocation()))
    {
      // if (VERBOSE) System.out.println("removing serialPort");
      remove (intfObj);
      curr_unit.removeSerialPort();
    }

    intfObj = curr_unit.getFirstSFPInterface();
    while (intfObj != null)
    {
      if (curr_panel == intfObj.getPanelLocation())
      {
        // if (VERBOSE) System.out.println("removing sfp"+intfObj.getSlot()+"."+intfObj.getPort());
        curr_unit.removeSFPInterface(intfObj.getSlot(), intfObj.getPort());
      }
      intfObj = curr_unit.getNextSFPInterface(intfObj);
    }

  }


  /*********************************************************************
  *
  * @functions removes ports from card and also removes corresponding 
  *
  *  port object from applet
  *
  * @purpose 
  *          
  * @param curr_unit 
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/

  public void removePorts(Card curr_card)
  {
    int p;
    Interface portobj;

    // if (DEBUG) System.out.println("Removing ports on card "+curr_card.getSlot());
    for (p=1; p<=curr_card.getNumPorts(); p++)
    {
      portobj = curr_card.getPort(p);
      remove(portobj);
      curr_card.removePort(p);
    }
  }


  /*********************************************************************
  *
  * @functions perform actions necessary to facilitate a toggle front<>back
  *
  * @purpose 
  *             
  * @param curr_unit 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  public void toggleUnitPanel(Unit unitObj)
  {
    int c, p, s;
    int curr_panel;
    int new_panel;
    int first_port;
    Card curr_card;
    Interface intfObj;

    toggle_in_progress = true;  // Don't allow paint until finished

    curr_panel = unitObj.getCurrentPanel();

    deactivateUnitPanel(unitObj, curr_panel);

    if (curr_panel == BACK_PANEL)
      new_panel = FRONT_PANEL;
    else
      new_panel = BACK_PANEL;

    activateUnitPanel(unitObj, new_panel);

    unitObj.setCurrentPanel(new_panel);

    toggle_in_progress = false;  // Don't allow paint until finished
    clickPaint = true;
  }



  /*********************************************************************
  *
  * @functions perform actions necessary to facilitate a toggle front<>back
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  public void deactivateUnit(Unit unitObj)
  {
    int curr_panel;

    curr_panel = unitObj.getCurrentPanel();
    deactivateUnitPanel(unitObj, curr_panel);
  }


  /*********************************************************************
  *
  * @functions perform actions necessary to facilitate a toggle front<>back
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  public void activateUnit(Unit unitObj)
  {
    int curr_panel;

    curr_panel = unitObj.getCurrentPanel();
    activateUnitPanel(unitObj, curr_panel);
  }

  /*********************************************************************
  *
  * @functions perform actions necessary to facilitate a toggle front<>back
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  public void deactivateUnitPanel(Unit unitObj, int unitPanel)
  {
    int c, p, s;
    int curr_panel;
    int first_port, first;
    int clickIndex;
    clickOption clickObj;
    Card curr_card;
    Interface intfObj;

    for (c=0; c<unitObj.getNumCards(); c++)
    {
      curr_card = unitObj.getCard(c);
      if (curr_card != null)
      {
         first_port = curr_card.getFirstPort();
         for (p=first_port; p<=curr_card.getNumPorts(); p++)
         {
           intfObj = curr_card.getPort(p);
           if (unitPanel == intfObj.getPanelLocation())
           {
             intfObj.deactivateInterface(this);
             remove(intfObj);
           }
         }
      }
    }

    first = unitObj.getFirstStackIntfNum();
    for (p=first; p<(unitObj.getNumStackIntf()+first); p++)
    {
      intfObj = unitObj.getStackIntf(p);
      if ((intfObj != null) && (unitPanel == intfObj.getPanelLocation()))
      {
        intfObj.deactivateInterface(this);
        remove(intfObj);
      }
    }

    intfObj = unitObj.getServicePort();
    if (intfObj != null)
    {
      if (unitPanel == intfObj.getPanelLocation())
      {
        intfObj.deactivateInterface(this);
        remove(intfObj);
      }
    }


    intfObj = unitObj.getSerialPort();
    if (intfObj != null)
    {
      if (unitPanel == intfObj.getPanelLocation())
      {
        intfObj.deactivateInterface(this);
        remove(intfObj);
      }
    }

    intfObj = unitObj.getFirstSFPInterface();
    while (intfObj != null)
    {
      if (unitPanel == intfObj.getPanelLocation())
      {
        intfObj.deactivateInterface(this);
        remove(intfObj);
      }
      intfObj = unitObj.getNextSFPInterface(intfObj);
    }

    clickIndex = unitObj.getClickObjectIndexFirst();
    while (clickIndex != -1)
    {
      clickObj = unitObj.getClickObject(clickIndex);  
      if (unitPanel == clickObj.getPanelLocation())
      {
        // if (DEBUG) System.out.println("deactivating "+clickObj.getDescription()+" click object "+clickIndex);
        clickObj.deactivateOption(this);
        remove(clickObj);
      }
      clickIndex = unitObj.getClickObjectIndexNext(clickIndex);
    }


  }


  /*********************************************************************
  *
  * @functions perform actions necessary to activate a unit panel
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  public void activateUnitPanel(Unit unitObj, int unitPanel)
  {
    int c, p, s;
    int first_port, first;
    int clickIndex;
    clickOption clickObj;
    Card curr_card;
    Interface intfObj;

    for (c=0; c<unitObj.getNumCards(); c++)
    {
      curr_card = unitObj.getCard(c);
      if (curr_card != null)
      {
        first_port = curr_card.getFirstPort();
        for (p=first_port; p<=curr_card.getNumPorts(); p++)
        {
          intfObj = curr_card.getPort(p);
          if (unitPanel == intfObj.getPanelLocation())
          {
            intfObj.activateInterface(this);
            add(intfObj);
          }
        }
      }
    }

    first = unitObj.getFirstStackIntfNum();
    for (p=first; p<(unitObj.getNumStackIntf()+first); p++)
    {
      intfObj = unitObj.getStackIntf(p);
      if ((intfObj != null) && (unitPanel == intfObj.getPanelLocation()))
      {
        intfObj.activateInterface(this);
        add(intfObj);
        // System.out.println("activateUnitPanel() Activated stack intf object for HiGig"+p);
        // System.out.println("activateUnitPanel() Link State for HiGig"+p+" is "+intfObj.getState());
      }
    }

    intfObj = unitObj.getServicePort();
    if (intfObj != null)
    {
      if (unitPanel == intfObj.getPanelLocation())
      {
        intfObj.activateInterface(this);
        add(intfObj);
      }
    }


    intfObj = unitObj.getSerialPort();
    if (intfObj != null)
    {
      if (unitPanel == intfObj.getPanelLocation())
      {
        intfObj.activateInterface(this);
        add(intfObj);
      }
    }

    intfObj = unitObj.getFirstSFPInterface();
    while (intfObj != null)
    {
      if (unitPanel == intfObj.getPanelLocation())
      {
        intfObj.activateInterface(this);
        add(intfObj);
      }
      intfObj = unitObj.getNextSFPInterface(intfObj);
    }


    clickIndex = unitObj.getClickObjectIndexFirst();
    while (clickIndex != -1)
    {
      clickObj = unitObj.getClickObject(clickIndex);  
      if (unitPanel == clickObj.getPanelLocation())
      {
        // if (DEBUG) System.out.println("activating "+clickObj.getDescription()+" click object "+clickIndex);
        clickObj.activateOption(this);
        add(clickObj);
      }
      clickIndex = unitObj.getClickObjectIndexNext(clickIndex);
    }

  }








  /*********************************************************************
  *
  * @functions create a new unit object
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  private Unit unitObjCreate(int    unit_num, 
                             int    num_slots, 
                             int    num_sfps, 
                             int    unit_status, 
                             int    unit_type, 
                             String strPrefix, 
                             int    current_panel_view,
                             int    unit_counter)
  {
    Unit            new_unit;
    Rectangle       getPrevUnit;
    Rectangle       getNextUnit;
    Rectangle       unitRectangle;
    Image           unitFrontImage; 
    Image           unitBackImage; 
    Image           indexImage;
    Image           preIndexImage;
    Point           unitLocation, loc;
    Dimension       dim;
    Dimension       unitSize;
    StringTokenizer st2;
    int             unitWidth, unitHeight, unitIdWidth, navUnitWidth;
    int             x_offset, y_offset;
    int             numStackIntf, firstStackIntf;


    getNextUnit = null;
    getPrevUnit = null;
    indexImage = null;
    preIndexImage = null;
    x_offset = 0;
    y_offset = 0;

    // if (DEBUG) System.out.println("creating new unit " + unit_num);


    toggleSupported = integerParameterGet(strPrefix+"_"+"TOGGLE_SUPPORTED", 0);

    if (isStacking == 1)
    {
      numStackIntf = integerParameterGet(strPrefix+"_"+"NUM_STACK_INTF", 0);
      firstStackIntf = integerParameterGet(strPrefix+"_"+"FIRST_STACK_INTF", 0);
    }
    else
    {
      numStackIntf = 0;
      firstStackIntf = 0;
    }

    unitWidth = integerParameterGet(strPrefix+"_width", 600);
    unitHeight = integerParameterGet(strPrefix+"_height", 80);


    unitSize = new Dimension(unitWidth, unitHeight);

    if (isStacking == 1)
    {
      unitIdWidth = integerParameterGet("stack_unitId_width", 30);
      indexImage = imageGet("stack_unit"+unit_num);
      preIndexImage = imageGet("stack_pre_unit"+unit_num);

      if (stackFullView != true)
      {
        navUnitWidth = integerParameterGet("stack_nav_width", 30);

        loc = locationGet("stack_prev"); 
        dim = dimensionGet("stack_prev_dim");
        getPrevUnit = new Rectangle(loc, dim);
        getPrevUnit.translate(unitIdWidth+unitWidth, 0);

        loc = locationGet("stack_next"); 
        dim = dimensionGet("stack_next_dim");
        getNextUnit = new Rectangle(loc, dim);
        getNextUnit.translate(unitIdWidth+unitWidth, 0);

        y_offset = 0;
      }
      else // Full Stack View
      {
        y_offset = unit_counter * unitHeight;
      }
      x_offset = unitIdWidth;
    }

    dim = new Dimension(unitWidth, unitHeight);
    unitLocation = new Point(x_offset, y_offset);  
    unitRectangle = new Rectangle(unitLocation, dim);


    // ---------------------------------------------------------------------
    //  Get the front<->back toggle areas on the box if supported
    // ---------------------------------------------------------------------

    unitFrontImage = imageGet(strPrefix+"_front");

    if (toggleSupported == 1)
      unitBackImage = imageGet(strPrefix+"_back");
    else
      unitBackImage = null; 


    // Assuming the card numbers are zero-based ... fix this TRGTRG  Also handle serial,service and stackintf
    new_unit = new Unit(unit_num,
                        num_slots,
                        0,                       // firstSlot
                        num_sfps,
                        unit_status, 
                        unit_type, 
                        FRONT_PANEL,
                        getNextUnit,
                        getPrevUnit,
                        numStackIntf,           // int numStackIntf 
                        firstStackIntf,         // int firstStackIntf
                        unitFrontImage, 
                        unitBackImage, 
                        x_offset, 
                        y_offset, 
                        unitRectangle,
                        indexImage,
                        preIndexImage);



    return new_unit;
  }


  /*********************************************************************
  *
  * @functions create a new interface object
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  private clickOption toggleObjCreate(String     strDescr,
                                      Unit       curr_unit,
                                      String     strPrefix, 
                                      int        panelLocation)
  {
    Point       loc;
    Dimension   dim;
    Rectangle   toggleRect;
    Image       toggleImage;
    Image       hilightImage;
    clickOption toggleObj;
    PopupMenu   optionMenu;
    String      clickAction;



    if (panelLocation == FRONT_PANEL)
    {
      loc = locationGet(strPrefix+"_toggle_to_back_loc");
      dim = dimensionGet(strPrefix+"_toggle_to_back_dim");
      toggleImage = imageGet(strPrefix+"_toggle_to_back_img");
      hilightImage = imageGet(strPrefix+"_toggle_to_back_img2");
    }
    else
    {
      loc = locationGet(strPrefix+"_toggle_to_front_loc");
      dim = dimensionGet(strPrefix+"_toggle_to_front_dim");
      toggleImage = imageGet(strPrefix+"_toggle_to_front_img");
      hilightImage = imageGet(strPrefix+"_toggle_to_front_img2");
    }
    loc.translate(curr_unit.getUnitXOffset(), curr_unit.getUnitYOffset()); 
    toggleRect = new Rectangle(loc, dim);

    optionMenu = null;
    clickAction = null;


    toggleObj = new clickOption(OPTION_TYPE_TOGGLE, 
                                this, 
                                panelLocation,
                                curr_unit,
                                toggleImage,
                                hilightImage,
                                loc,
                                toggleRect,
                                optionMenu,
                                strDescr,
                                clickAction,
                                win);

    return toggleObj;
  }


  /*********************************************************************
  *
  * @functions create a new interface object
  *
  * @purpose 
  *             
  * @param unit object 
  *
  * @returns none
  *
  * @notes none
  *
  * @end       
  *
  *********************************************************************/

  private Interface interfaceObjCreate(int    intfType,
                                       int    unit_num, 
                                       int    slot_num, 
                                       int    port_num,
                                       Unit   curr_unit,
                                       String strPrefix, 
                                       String slot_type, 
                                       int    pluggable,
                                       int    panelLocation)
  {
    Interface intfObj;
    Image     baseImage = null;
    Image     linkUpImage = null;
    Image     linkDownImage = null;
    Image     linkDisabledImage = null;
    Image     fpsUpImage = null;
    Image     fpsDownImage = null;
    Image     fpsDisabledImage = null;
    int       unitXLoc;
    int       unitYLoc;
    int       intf_type;
    int       linkState;
    int       stackIntfFlags = 0;
    int       fpsSupport = 0;
    int       fpsModeSupport = 0;
    int       stackModeSupport = 0;
    int       connType = 0;
    boolean   fpsMode = false;
    Image     poeUpImage = null;
    Image     poeDownImage = null;
    Image     poeDisabledImage = null;
    boolean   poeMode = false;
    Image     poeDetachedImage = null;
    int       poeSupport = 0;
    boolean   stackMode = false;
    int       sfpSupported = 0;
    Point     baseLocation;
    Point     linkOffset;
    String    intfPrefix = null;
    String    clickAction = null;   
    String    parsePrefix;
    String    strInfo = null;
    Dimension dim;
    Rectangle baseRectangle;
    PopupMenu intfmenu = null;      // the PopupMenu 
    Card      cardObj;


    unitXLoc = curr_unit.getUnitXOffset();
    unitYLoc = curr_unit.getUnitYOffset();
    cardObj = curr_unit.getCard(slot_num);

    switch (intfType)
    {
    case INTF_PHYSICAL:
      strInfo = new String("Link state for "+unit_num+"/"+slot_num+"/"+port_num);
      break;
    case INTF_SFP:
      strInfo = new String("Link state for SFP "+unit_num+"/"+slot_num+"/"+port_num);
      break;
    case INTF_STACK:
      strInfo = new String("Link state for Stack Interface "+unit_num+"/"+slot_num+"/"+port_num);
      break;
    case INTF_SERIAL:
      strInfo = new String("Link state for Serial Port");
      break;
    case INTF_SERVICE:
      strInfo = new String("Link state for Service Port");
      break;
    default:
      strInfo = new String("Link state for unknown interface type");
      break;
    }

    linkState = readData(strInfo);  // Get the link state 

    stackIntfFlags = linkState & APPLET_IFLAG_STACKING_MASK;
    fpsSupport = linkState & APPLET_IFLAG_FPS_SUPPORT;
    fpsModeSupport = linkState & APPLET_IFLAG_FPS_MODE_SUPPORT;
    if ((linkState & APPLET_IFLAG_FPS_MODE) == APPLET_IFLAG_FPS_MODE)
      fpsMode = true;
    else
      fpsMode = false;
    stackModeSupport = linkState & APPLET_IFLAG_STACK_MODE_SUPPORT;
    if ((linkState & APPLET_IFLAG_STACK_MODE_HIGIG) == APPLET_IFLAG_STACK_MODE_HIGIG)
      stackMode = true;
    else
      stackMode = false;

    poeSupport = linkState & APPLET_IFLAG_POE_MODE;
    if ((linkState & APPLET_IFLAG_POE_MODE) == APPLET_IFLAG_POE_MODE)
      poeMode = true;
    else
      poeMode = false;

    linkState &= 0x000000FF;  // Mask off all but linkstate info

    switch (intfType)
    {
    case INTF_PHYSICAL:
      parsePrefix = new String(strPrefix+"_port"+slot_type+"."+port_num);
      if (pluggable != 1)
      sfpSupported = integerParameterGet(parsePrefix+"_sfp", 0);
      if (panelLocation == DYNAMIC_PANEL)
      {
        if (integerParameterGet(parsePrefix+"_panel", 0) == 1)  
          panelLocation = FRONT_PANEL;
        else
          panelLocation = BACK_PANEL;
      }
      break;
    case INTF_SFP:
      parsePrefix = new String(strPrefix+"_sfp"+slot_type+"."+port_num);
      if (integerParameterGet(parsePrefix+"_panel", 0) == 1)  
        panelLocation = FRONT_PANEL;
      else
        panelLocation = BACK_PANEL;
      break;
    case INTF_STACK:
      parsePrefix = new String(strPrefix+slot_type+port_num);
      if (integerParameterGet(parsePrefix+"_panel", 0) == 1)
        panelLocation = FRONT_PANEL;
      else
        panelLocation = BACK_PANEL;
      break;
    case INTF_SERIAL:
      parsePrefix = new String(strPrefix+slot_type);
      if (integerParameterGet(parsePrefix+"_panel", 0) == 1)
        panelLocation = FRONT_PANEL;
      else
        panelLocation = BACK_PANEL;
      break;
    case INTF_SERVICE:
      parsePrefix = new String(strPrefix+slot_type);
      if (integerParameterGet(parsePrefix+"_panel", 0) == 1)
        panelLocation = FRONT_PANEL;
      else
        panelLocation = BACK_PANEL;
      break;
    default:
      parsePrefix = new String("parsePrefix_parse_error");
      panelLocation = FRONT_PANEL;
      break;
    }

    switch (intfType)
    {
    case INTF_PHYSICAL:
    case INTF_SFP:
      intfPrefix = stringParameterGet(parsePrefix+"_type");
      if (intfPrefix == null)
        intfPrefix = new String("rj45");
      if (fpsSupport == APPLET_IFLAG_FPS_SUPPORT)
      {
        fpsUpImage = imageGet(strPrefix+"_"+intfPrefix+"_fps_up");
        fpsDownImage = imageGet(strPrefix+"_"+intfPrefix+"_fps_down");
        fpsDisabledImage = imageGet(strPrefix+"_"+intfPrefix+"_fps_disabled");
      }

      if ((stackIntfFlags & APPLET_IFLAG_FPS_SUPPORT) == APPLET_IFLAG_FPS_SUPPORT)
      {
        if ((stackIntfFlags & APPLET_IFLAG_FPS_MODE_SUPPORT) == APPLET_IFLAG_FPS_MODE_SUPPORT)
          intfmenu = fpsIntfPopupMenu;
        else
          intfmenu = stackIntfPopupMenu;
      }
      else if ((stackIntfFlags & APPLET_IFLAG_STACK_MODE_HIGIG) == APPLET_IFLAG_STACK_MODE_HIGIG)
      {
        intfmenu = stackIntfPopupMenu;
      }
      else if ((stackIntfFlags & APPLET_IFLAG_STACK_MODE_10G) == APPLET_IFLAG_STACK_MODE_10G)
      {
        intfmenu = fpsIntfPopupMenu;
      }
      else
      {
        intfmenu = portPopupMenu;
      }

      baseLocation = locationGet(parsePrefix+"_loc");
      if (pluggable == 1)
      {
        baseLocation.translate(cardObj.getXOffset(), cardObj.getYOffset());
      }

      if (integerParameterGet(parsePrefix+"_flip", 0) == 1)
        baseImage = imageGet(strPrefix+"_"+intfPrefix+"_base_flip");
      else
        baseImage = imageGet(strPrefix+"_"+intfPrefix+"_base");

      if (poeMode)
      {
        poeUpImage = imageGet(strPrefix+"_"+intfPrefix+"_poe_up");
        poeDownImage = imageGet(strPrefix+"_"+intfPrefix+"_poe_down");
        poeDisabledImage = imageGet(strPrefix+"_"+intfPrefix+"_poe_disabled");
        poeDetachedImage = imageGet(strPrefix+"_"+intfPrefix+"_poe_disabled");
      }

      linkUpImage = imageGet(strPrefix+"_"+intfPrefix+"_up");
      linkDownImage = imageGet(strPrefix+"_"+intfPrefix+"_down");
      linkDisabledImage = imageGet(strPrefix+"_"+intfPrefix+"_disabled");
      linkOffset = locationGet(strPrefix+"_"+intfPrefix+"_link_offset");
      dim = dimensionGet(strPrefix+"_"+intfPrefix+"_dim");
      connType = connectorfTypeGet(intfPrefix);
      break;

    case INTF_STACK:
      intfPrefix = stringParameterGet(parsePrefix+"_type");
      if (intfPrefix == null)
        intfPrefix = new String("xaui");
      intfmenu = stackIntfPopupMenu;
      baseLocation = locationGet(parsePrefix+"_loc");
      if (pluggable == 1)
      {
        baseLocation.translate(cardObj.getXOffset(), cardObj.getYOffset());
      }
      stackIntfFlags = APPLET_IFLAG_STACK_MODE_ONLY;
      baseImage = imageGet(strPrefix+"_"+intfPrefix+"_base");
      linkUpImage = imageGet(strPrefix+"_"+intfPrefix+"_up");
      linkDownImage = imageGet(strPrefix+"_"+intfPrefix+"_down");
      linkDisabledImage = imageGet(strPrefix+"_"+intfPrefix+"_disabled");
      linkOffset = locationGet(strPrefix+"_"+intfPrefix+"_link_offset");
      dim = dimensionGet(strPrefix+"_"+intfPrefix+"_dim");
      connType = connectorfTypeGet(intfPrefix);
      break;

    case INTF_SERIAL:
      baseImage = imageGet(parsePrefix+"_base");
      linkUpImage = imageGet(parsePrefix+"_up");
      linkDownImage = imageGet(parsePrefix+"_down");
      linkDisabledImage = baseImage;

      baseLocation = locationGet(parsePrefix+"_loc");

      linkOffset = locationGet(parsePrefix+"_link_offset");
      dim = dimensionGet(parsePrefix+"_dim");
      clickAction = stringParameterGet(parsePrefix+"_action");
      connType = INTF_TYPE_SERIAL_PORT;
      break;

    case INTF_SERVICE:
      baseImage = imageGet(parsePrefix+"_base");
      linkUpImage = imageGet(parsePrefix+"_up");
      linkDownImage = imageGet(parsePrefix+"_down");
      linkDisabledImage = baseImage;

      baseLocation = locationGet(parsePrefix+"_loc");

      linkOffset = locationGet(parsePrefix+"_link_offset");
      dim = dimensionGet(parsePrefix+"_dim");
      clickAction = stringParameterGet(parsePrefix+"_action");
      connType = INTF_TYPE_SERVICE_PORT;
      break;

    default:
      baseLocation = new Point(0,0);
      linkOffset = new Point(0,0);
      dim = new Dimension(0,0);
      break;
    }

    baseLocation.translate(unitXLoc, unitYLoc); 
    baseRectangle = new Rectangle(baseLocation, dim);

    if (intfType == INTF_STACK)
    {
      // if (DEBUG) System.out.println("Creating stacking interface " + unit_num + "/" + slot_num + "/" + port_num +" at ("+baseLocation.getX()+","+baseLocation.getY()+")" );
    }

    intfObj = new Interface(unit_num, 
                            slot_num, 
                            port_num, 
                            linkState, 
                            sfpSupported, 
                            stackIntfFlags, 
                            connType, 
                            panelLocation, 
                            curr_unit, 
                            baseImage, 
                            poeMode,
                            fpsMode,
                            poeUpImage,
                            poeDownImage,
                            poeDisabledImage,
                            poeDetachedImage,
                            linkUpImage, 
                            linkDownImage, 
                            linkDisabledImage, 
                            fpsUpImage, 
                            fpsDownImage, 
                            fpsDisabledImage, 
                            baseLocation, 
                            linkOffset, 
                            baseRectangle, 
                            intfmenu, 
                            clickAction, 
                            win);


    // if (VERBOSE) System.out.println("adding interface " + unit_num + "." + slot_num + "." + p );
    return intfObj;
  }


  /*********************************************************************
  *
  * @functions updateVectorStates
  *
  * @purpose Update Vector containing all port info with the port states
  *          received through the socket.
  *          
  * @param none
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public  int updateVectorStates()
  {
    /* Need to add Classes for Units and Cards
       These will be in addition to the Data (port) class 
       Card class will contain ids of port objects
       Unit Class will contain ids of card objects
       System class containing units ?
     */

    int repaint_needed = 0;
    int i, u, c, p;
    int end_of_data;
    int trace_mode;
    int unit_num;
    int unused;
    int num_units;
    int unit_type;
    int unit_status;
    int num_slots;
    int num_sfps;
    int slot_num;
    int card_id;
    int card_status;
    int num_ports;
    int pluggable;
    int conn_type;
    int first_port_num;
    int linkState, sfpLinkState, intf_type, intf_index;
    int service_port_state;
    int serial_port_state;
    int service_port_present;
    int serial_port_present;
    int sfpSupported;
    int sfp_interfaces_present;
    int panelLocation;
    int sfpPanelLocation;
    int slotPanel;
    int slotXOffset;
    int slotYOffset;
    boolean exists = false;
    boolean reload_needed = false;
    StringTokenizer st2;
    String slot_type = null;
    Interface portobj, current, sfpObj;
    Card cardobj, curr_card;
    Unit unitobj, curr_unit;
    Image baseImage, linkUpImage, linkDownImage, linkDisabledImage, slotImage;
    Image fpsUpImage, fpsDownImage, fpsDisabledImage;
    Image poeUpImage, poeDownImage, poeDisabledImage, poeDetachedImage;
    Point baseLocation, linkOffset, slotLocation;
    Rectangle baseRectangle, slotRectangle;
    Dimension dim;
    Point loc;
    int   unitXLoc, unitYLoc;
    int current_panel_view;
    int initialNumUnits;
    String intfPrefix = null;
    String strInfo = null;
    int poeSupport = 0;
    boolean poeMode = false;
    int fpsSupport;
    int fpsModeSupport;
    boolean fpsMode = false;
    int currentFPSMode;
    int stackIntfFlags;
    PopupMenu intfmenu;  // the PopupMenu for ports
    clickOption toggle2BackObj;
    clickOption toggle2FrontObj;

    // Request the APPLET Data update

    //   update_in_progress = true;

    win = JSObject.getWindow(this);   

    try
    {
      dos.writeInt(APPLET_CODE);            
      dos.flush();
    }
    catch (Throwable t)
    {
      System.out.println("Error sending applet command ... update aborted");
      return -1;
    }

    initialNumUnits = stack.getNumUnits();

    curr_unit = stack.getFirstUnit();

    while (curr_unit != null)
    {
      curr_unit.setFlagged(false);             
      i = curr_unit.getUnitID();
      curr_unit = stack.getNextUnit(i);
    }

    trace_mode = readData("Trace Mode");
    if ((trace_mode & MODE_READ_DATA_TRACE) == MODE_READ_DATA_TRACE)
      READ_DATA_TRACE = true;
    else
      READ_DATA_TRACE = false;

    isXUI = readData("XUI Support");
    if (isXUI == -1)
      return -1;
    isStacking = readData("Stacking Support");
    if (isStacking == -1)
      return -1;
    num_units = readData("Number of units");
    if (num_units == -1)
      return -1;
    // if (DEBUG) System.out.println("Num units is " + num_units);

    if (num_units == 0) /* Ignore this update and wait for the next one */
      return -1;

    for (u=0; u<num_units; u++)
    {
      String clickAction = null;
      String strPrefix;
      String uType;

      try
      {
        unit_num    = readData("Unit ID");          // unit number       
        if (unit_num == END_OF_DATA)
        {
          // if (DEBUG) System.out.println("End of data found prematurely");
          break;
        }
        // if (DEBUG) System.out.println("Reading data for Unit"+ unit_num);
        unit_type     = readData("Unit Type");          // unit identification        
        // based on the unit id we set the prefix for the parameter gets

        uType = new String(Long.toHexString(unit_type).toUpperCase());
        strPrefix = new String(company + uType);
        
        // unit_action = readData();           (0) - update, (1) - add, (2) - remove  TRGTRG add/remove unit or card code needed
        unit_status = readData("Unit Status");          // Preconfigured or physically present
        num_sfps    = readData("Number of SFPs");          // number of sfp's in this unit
        num_slots   = readData("Number of Slots");          // number of cards (slots) in this unit

        // if (VERBOSE) System.out.println("Unit status: "+unit_status+"  sfps: "+num_sfps+"  slots: "+num_slots);
      }
      catch (Throwable t)
      {
        System.out.println("Error reading unit " + u +" info  ... update aborted");
        return -1;
      }

      /*check whether a unit with the particular unit number exists and if so whether*/
      /*the unit is the same as that one intimated from the data stream*/
      curr_unit = null;
      exists = false;
      if (stack.unitExists(unit_num))
      {
        curr_unit = stack.getUnit(unit_num);
        if (curr_unit.getType() != unit_type)
        {
          removeSlotPorts(curr_unit);
          //remove the unit
          stack.removeUnit(unit_num);
          exists = false;
        }
        else
        {
          exists = true;
          // if (VERBOSE) System.out.println("Unit "+unit_num+" exists");
        }
      }
      if (exists == true)
      {
        curr_unit = stack.getUnit(unit_num);
        /*check if pre-configured unit has become physically present and vice versa*/
        if (unit_status != curr_unit.getState())
        {
          repaint_needed = 1;
          curr_unit.setState(unit_status);
        }

        current_panel_view = curr_unit.getCurrentPanel();
        curr_unit.setFlagged(true);
      }
      else  /*if new unit to be created*/
      {
        current_panel_view = FRONT_PANEL;
        curr_unit = unitObjCreate(unit_num, num_slots, num_sfps, unit_status, unit_type, strPrefix, current_panel_view, u);

        if (curr_unit != null)
        {
          /*setting up displayedUnit when there is no displayed unit e*/
          if (displayedUnit == 0)
          {
            displayedUnit = unit_num;
            // if (VERBOSE) System.out.println("displayedUnit is unit "+unit_num);
          }

          // if (DEBUG) System.out.println("adding unit "+unit_num+" to the stack");
          stack.addUnit(unit_num, curr_unit);


          if (toggleSupported == 1)
          {
            toggle2BackObj = toggleObjCreate("Toggle to back", 
                                              curr_unit,
                                              strPrefix, 
                                              FRONT_PANEL);
            curr_unit.addToggleToBack(toggle2BackObj);


            toggle2FrontObj = toggleObjCreate("Toggle to front", 
                                               curr_unit,
                                               strPrefix, 
                                               BACK_PANEL);
            curr_unit.addToggleToFront(toggle2FrontObj);


            if (((displayedUnit == unit_num) || (stackFullView == true)) && (current_panel_view == FRONT_PANEL))
            {
              toggle2BackObj.activateOption(this);
              add(toggle2BackObj);
            }

            if (((displayedUnit == unit_num) || (stackFullView == true)) && (current_panel_view == BACK_PANEL))
            {
              toggle2FrontObj.activateOption(this);
              add(toggle2FrontObj);
            }

          }



          repaint_needed = 1;
          reload_needed = true;
        }
        else
        {
          curr_unit = stack.getFirstUnit();
        }

      }

      unitXLoc = curr_unit.getUnitXOffset();
      unitYLoc = curr_unit.getUnitYOffset();

      for (c=0; c<num_slots; c++)
      {
        slot_num       = readData("Slot Number");   // slot number
        card_status    = readData("Card Status");   // populated or empty
        card_id        = readData("Card ID");   // card identification
        num_ports      = readData("Number of Ports");   // number of ports in this slot
        first_port_num = readData("First Port Number");   // first port number in this slot

        exists = false;
        /*if num ports of card not same as that which is present, remove card*/
        if (curr_unit.cardExists(slot_num))
        {
          curr_card = curr_unit.getCard(slot_num);
          if (num_ports != curr_card.getNumPorts())
          {
            removePorts(curr_card);
            curr_unit.removeCard(slot_num);
          }
          else
          {
            exists = true;
          }
        }
        else
        {
          curr_card = null;
        }

        // Does this card already exist
        if (exists == true)  // TRGTRG also need to check the card_id in this slot for change??
        {
          curr_card.setState(card_status);


          for (p=first_port_num; p<(num_ports+first_port_num); p++)
          {
            current = curr_card.getPort(p);

            strInfo = new String("Link state for "+unit_num+"/"+slot_num+"/"+p);
            linkState = readData(strInfo);  // Get the link state 
            stackIntfFlags = linkState & APPLET_IFLAG_STACKING_MASK;
            fpsSupport = linkState & APPLET_IFLAG_FPS_SUPPORT;
            fpsModeSupport = linkState & APPLET_IFLAG_FPS_MODE_SUPPORT;
            if ((linkState & APPLET_IFLAG_FPS_MODE) == APPLET_IFLAG_FPS_MODE)
              fpsMode = true;
            else
              fpsMode = false;

            poeSupport = linkState & APPLET_IFLAG_POE_MODE;
            if ((linkState & APPLET_IFLAG_POE_MODE) == APPLET_IFLAG_POE_MODE)
              poeMode = true;
            else
              poeMode = false;

            // if (DEBUG) if (fpsSupport == APPLET_IFLAG_FPS_SUPPORT) System.out.println("Interface " + unit_num + "." + slot_num + "." + p + " supports FPS");
            // if (DEBUG) if (fpsMode == APPLET_IFLAG_FPS_MODE) System.out.println("Interface " + unit_num + "." + slot_num + "." + p + " has FPS mode ENABLED");
            linkState &= 0x000000FF;  // Mask off all but linkstate info

            // FPS Design - fps mode change
            // - change current fps mode
            // - update link images appropriately 
            // - set portpopup menu accordingly 

            if (current.getFPSMode() != fpsMode)
              current.setFPSMode(fpsMode);


            // poe mode change
            // - change current mode mode
            if (current.getPoeMode() != poeMode)
              current.setPoeMode(poeMode);

            if (current.getState() != linkState)
            {
              current.setState(linkState);
            }


            // SFP Check 
            if (current.getSFPSupport() == 1)
            {
              sfpObj = curr_unit.getSFPInterface(slot_num, p);
              
              strInfo = new String("Link state for SFP "+unit_num+"/"+slot_num+"/"+p);
              sfpLinkState = readData(strInfo);  // Get the link state 
              sfpLinkState &= 0x000000FF;  // Mask off all but linkstate info

              if (sfpObj.getFPSMode() != fpsMode)
                sfpObj.setFPSMode(fpsMode);


              if (sfpObj.getState() != sfpLinkState)
              {
                sfpObj.setState(sfpLinkState);
              }
            }
          }

        }
        else // Create new card and all the ports
        {
          if (card_status == 1)  /* Card present */
          {
          slotImage     = null;
          slotLocation  = null;
          slotRectangle = null;
            slot_type     = null;
            slotXOffset   = 0;
            slotYOffset   = 0;
          // }
          slotPanel = integerParameterGet(strPrefix+"_slot"+slot_num+"_location", 3);
          if (slotPanel == 1)
            panelLocation = FRONT_PANEL;
          else if (slotPanel == 2)
            panelLocation = BACK_PANEL;
          else
            panelLocation = DYNAMIC_PANEL;

            pluggable = integerParameterGet(strPrefix+"_slot"+slot_num+"_pluggable", 0);
            if (pluggable == 1)
            {
              if (card_status == 1) /* Populated */
              {
                slot_type = new String(Long.toHexString(card_id).toUpperCase());
                slotImage = imageGet(strPrefix+"_slot"+slot_type+"_img");   /* eg. bcmE304_Slot42D2_img */
                slotLocation  = locationGet(strPrefix+"_slot"+slot_num+"_coord");     /* eg. bcmE304_slot2_coord */
                dim = dimensionGet(strPrefix+"_slot"+slot_num+"_dim");      /* eg. bcmE304_slot2_dim */
                slotRectangle = new Rectangle(slotLocation, dim);
                slotXOffset = xLocGet(strPrefix+"_slot"+slot_num+"_coord");
                slotYOffset = yLocGet(strPrefix+"_slot"+slot_num+"_coord");
              }
            }
            else
            {
          slot_type = stringParameterGet(strPrefix+"_slot"+slot_num+"_type");
            }

          cardobj = new Card(unit_num, slot_num, num_ports, first_port_num, card_status, 
                               pluggable, card_id, panelLocation, slotImage, slotXOffset, slotYOffset, 
                               slotLocation, slotRectangle); 
          curr_unit.addCard(slot_num, cardobj);
          curr_card = cardobj;

          for (p=first_port_num; p<(num_ports+first_port_num); p++)
          {
            portobj = interfaceObjCreate(INTF_PHYSICAL, unit_num, slot_num, p, curr_unit, strPrefix, slot_type, pluggable, panelLocation);

            curr_card.addPort(p, portobj);

            if (((displayedUnit == unit_num) || (stackFullView == true)) && (portobj.getPanelLocation() == current_panel_view))
            {
              portobj.activateInterface(this);
              add(portobj);
            }

            sfpSupported = portobj.getSFPSupport();

            if (sfpSupported == 1)
            {
              sfpObj = interfaceObjCreate(INTF_SFP, 
                                          unit_num, 
                                          slot_num, 
                                          p, 
                                          curr_unit, 
                                          strPrefix, 
                                          slot_type, 
                                          pluggable, 
                                          panelLocation);

              curr_unit.addSFPInterface(sfpObj);

              sfpPanelLocation = sfpObj.getPanelLocation();

              if (((displayedUnit == unit_num) || (stackFullView == true)) && (sfpPanelLocation == current_panel_view))
              {
                sfpObj.activateInterface(this);
                add(sfpObj);
              }
            }

            // if (VERBOSE) System.out.println("adding interface " + unit_num + "." + slot_num + "." + p );
          }

          repaint_needed = 1;
        }
        }

        // if (VERBOSE) System.out.println("Adding data for unit-"+unit_num+" card-"+slot_num+" with "+num_ports+" ports");

      }

      // Look for dedicated stacking interfaces 

      if (isStacking == 1) 
      {
        try
        {
          // ---------------------------------------------------------------------
          //  Dedicated Stacking Interfaces
          // ---------------------------------------------------------------------
          int numStackInterfaces = readData("Number of dedicated stack interfaces");

          for (p=1; p<(numStackInterfaces+1); p++)
          {
            if (curr_unit.getStackIntf(p) == null) /* Need to create the stack interface */
            {
              // if (DEBUG) System.out.println("numStackInterfaces = "+numStackInterfaces+" for unit"+unit_num);
              slot_num = 0;
              pluggable = 0;
              panelLocation = BACK_PANEL;  /* TRGTRG this should move in to interfaceObjCreate() */
              portobj = interfaceObjCreate(INTF_STACK, 
                                           unit_num, 
                                           slot_num, 
                                           p, 
                                           curr_unit, 
                                           strPrefix, 
                                           "_higig", 
                                           pluggable, 
                                           panelLocation);

              curr_unit.addStackIntf(p, portobj);  
              if (((displayedUnit == unit_num) || (stackFullView == true)) && 
                  (portobj.getPanelLocation() == current_panel_view))
              {
                // if (DEBUG) System.out.println("Activating stack interface "+unit_num+".0."+p);
                portobj.activateInterface(this);
                add(portobj);
              }
            }
            else
            {
              strInfo = new String("Link state for dedicated stack interface "+unit_num+"/"+0+"/"+p);
              linkState = readData(strInfo);
              portobj = curr_unit.getStackIntf(p);
              if ((portobj != null) && (portobj.getState() != linkState))
                portobj.setState(linkState);
            }
          }
        }
        catch (Throwable t)
        {
          System.out.println("Error reading dedicated stack interface data");
          return -1;
        }
      }



      try
      {
        // ---------------------------------------------------------------------
        //  Service Port
        // ---------------------------------------------------------------------
        int servicePortPresent = readData("Service Port Present");

        if (servicePortPresent == 1)   /* Service Port Present */
        {
          portobj = curr_unit.getServicePort();
          if (portobj == null)
          {
            pluggable = 0;
            panelLocation = 0;  // Will be determined inside interfaceObjCreate()
            portobj = interfaceObjCreate(INTF_SERVICE, 
                                         unit_num, 
                                         0, 
                                         0, 
                                         curr_unit, 
                                         strPrefix, 
                                         "_service_port", 
                                         pluggable, 
                                         panelLocation);

            curr_unit.addServicePort(portobj);  
            if (((displayedUnit == unit_num) || (stackFullView == true)) && 
                (portobj.getPanelLocation() == current_panel_view))
            {
              portobj.activateInterface(this);
              add(portobj);
            }

            //validate();
            repaint_needed = 1;
          }
          else
          {
            linkState = readData("Service Port Link State");
            if (portobj.getState() != linkState)
              portobj.setState(linkState);
          }
        }


        // ---------------------------------------------------------------------
        //  Serial Port
        // ---------------------------------------------------------------------
        int serialPortPresent = readData("Serial Port Present");
        if (serialPortPresent == 1)   /* Serial Port Present */
        {
          portobj = curr_unit.getSerialPort();
          if (portobj == null)
          {
            pluggable = 0;
            panelLocation = 0;  // Will be determined inside interfaceObjCreate()
            portobj = interfaceObjCreate(INTF_SERIAL, 
                                         unit_num, 
                                         0, 
                                         0, 
                                         curr_unit, 
                                         strPrefix, 
                                         "_serial_port", 
                                         pluggable, 
                                         panelLocation);

            curr_unit.addSerialPort(portobj);  
            if (((displayedUnit == unit_num) || (stackFullView == true)) && 
                (portobj.getPanelLocation() == current_panel_view))
            {
              portobj.activateInterface(this);
              add(portobj);
            }

            //validate();
            repaint_needed = 1;
          }
          else
          {
            linkState = readData("Serial Port Link State");
            if (portobj.getState() != linkState)
              portobj.setState(linkState);
          }
        }


      }
      catch (Throwable t)
      {
        System.out.println("Error reading service, serial or sfp interface data");
        return -1;
      }
    }

    end_of_data = readData("End of Data");
    if (end_of_data != END_OF_DATA)    
    {
      System.out.println("Error: End of data was not received ... will try to re-sync.");
      for (i=0; i<(NUMBYTES*2); i++)
      {
        end_of_data = readData("Looking for End of Data");
        if (end_of_data == END_OF_DATA)    
          break;
      }
    }

    if (update_in_progress == true)
    {
      repaint_needed = 1;
      update_in_progress = false;
    }

    /*to see if any units have been removed*/


    unitobj = stack.getFirstUnit();
    while (unitobj != null)
    {
      unit_num = unitobj.getUnitID();

      if (unitobj.getFlagged() == false)
      {
        repaint_needed = 1;

        // if (DEBUG) System.out.println("removing unit "+unit_num+" from the stack");

        removeSlotPorts(unitobj);

        stack.removeUnit(unit_num);
        repaint_needed = 1;
        reload_needed = true;
      }
      unitobj = stack.getNextUnit(unit_num);
    }

    if ((isStacking == 1) && (stackFullView == true) && (reload_needed == true) && (initialNumUnits != 0))
    {
      // if (DEBUG) System.out.println("reload_needed == true");
      win.eval("window.location.reload(true);");
    }

    return repaint_needed;

  }

  /*********************************************************************
  *
  * @functions paint
  *
  * @purpose "paints" the correct configuration of slots and/or ports to 
  *          the applet according to parameters in MapApplet.html
  * @param Graphics g
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void paint(Graphics g)
  {
    /* Paints the images for the applet */
    Unit      curr_unit, prev_unit;
    Interface portobj;
    Card      curr_card;
    int       unit_y_coord;
    int       unit_x_coord;
    int       slot, first_slot, num_slots;
    Card      cardObj;
    boolean   toggleNow = false;
    clickOption clickObj;
    Image     img;

    unit_x_coord = 0;
    unit_y_coord = 0;

    /* Paint the background image
                 unit base image(s)
                 base interface images
                 link state images
                 
      TRGTRG Consider conditional repaint for only those things that change           
                 
     */

    if (stackFullView)
      curr_unit = (Unit)stack.getFirstUnit();
    else
      curr_unit = (Unit)stack.getUnit(displayedUnit);  


    if (!update_in_progress && !toggle_in_progress)
    {
      /* draws the background -- TRGTRG needs to change for stacking/non-stacking code */
      //  g.drawImage(stack.getSystemBackground(), 0, 0, this);     /* Stack panel image */

      /* now draw all the units, mgmt unit on top */

      if (curr_unit == null)
      {
        // if (DEBUG) System.out.println("current unit is null and displayed unit read "+displayedUnit);
        curr_unit = (Unit)stack.getFirstUnit();
        if (curr_unit != null)
        {
          displayedUnit = curr_unit.getUnitID();
          // if (DEBUG) System.out.println("Setting displayed unit to "+displayedUnit);
        }
        else
        {
          // if (DEBUG) System.out.println("Get first unit returned null");
        }
      }

      while (curr_unit != null)
      {
        // if (DEBUG) System.out.println("updating image for Unit "+curr_unit.getUnitID()+" of "+stack.getNumUnits()+" units");

        unit_x_coord = curr_unit.getUnitXOffset();
        unit_y_coord = curr_unit.getUnitYOffset();

        // Check to see what view is currently selected, front or back ... 
        if (curr_unit.getCurrentPanel() == BACK_PANEL)
        {
          // if (VERBOSE) System.out.println("Drawing back image for unit"+curr_unit.getUnitID()+" at ("+unit_x_coord+","+unit_y_coord+")");
          img = curr_unit.getUnitBackImage();
          if (img != null)
            g.drawImage(img, unit_x_coord, unit_y_coord, this);
          else
            System.out.println("BACK Image NULL");
        }
        else
        {
          // if (VERBOSE) System.out.println("Drawing front image for unit"+curr_unit.getUnitID()+" at ("+unit_x_coord+","+unit_y_coord+")");
          img = curr_unit.getUnitFrontImage();
          if (img != null)
            g.drawImage(img, unit_x_coord, unit_y_coord, this);
          else
            System.out.println("FRONT Image NULL");
        }

        /* Paint pluggable slot images here */
        num_slots = curr_unit.getNumCards();
        first_slot = curr_unit.getFirstCard();
        for (slot=first_slot; slot<num_slots; slot++)
        {
          if (curr_unit.cardExists(slot))
          {
            cardObj = curr_unit.getCard(slot);
            if (cardObj != null)
            {
              if ((cardObj.getPluggable() == 1) && (cardObj.getPanelLocation() == curr_unit.getCurrentPanel()))
                g.drawImage(cardObj.getSlotImage(), 
                            cardObj.getXOffset() + unit_x_coord, 
                            cardObj.getYOffset() + unit_y_coord, 
                            this);
            }
          }
        }

        if (isStacking == 1)
        {
          // Check to see whether unit is pre-configured or not
          if (curr_unit.getState() == 1)
            g.drawImage(curr_unit.getIndexImage(), 0, unit_y_coord, this);
          else
            g.drawImage(curr_unit.getPreIndexImage(), 0, unit_y_coord, this);
          // if (VERBOSE) System.out.println("Drawing unit index for unit"+curr_unit.getUnitID()+" at (0,"+unit_y_coord+")");

          if (stackFullView != true)
          {
            if (stack.getPrevUnit(displayedUnit) != null)
            {
              g.drawImage(navImagePrev, 
                          (int)curr_unit.getUnitWidth() + unit_x_coord, 
                          unit_y_coord, this);
            }
            else
            {
              g.drawImage(navImageBlank, 
                          (int)curr_unit.getUnitWidth() + unit_x_coord, 
                          unit_y_coord, this);
            }

            if (stack.getNextUnit(displayedUnit) != null)
            {
              g.drawImage(navImageNext, 
                          (int)curr_unit.getUnitWidth() + unit_x_coord, 
                          (int)(curr_unit.getUnitHeight() / 2) + unit_y_coord, 
                          this);
            }
            else
            {
              g.drawImage(navImageBlank, 
                          (int)curr_unit.getUnitWidth() + unit_x_coord, 
                          (int)(curr_unit.getUnitHeight() / 2) + unit_y_coord, 
                          this);
            }
            g.drawImage(stackLinkImage, 0, (int)(curr_unit.getUnitHeight()), this);

            break;
          }
          else
          {
            prev_unit = curr_unit;
            curr_unit = (Unit)stack.getNextUnit(prev_unit.getUnitID());
          }
        }
        else
        {
          break;
        }
      }
    }
    super.paint(g); 
  }

  /*********************************************************************
  *
  * @functions mouseClicked
  *
  * @purpose Displays the web page of a clicked area, or a popup menu 
  *          if the mouse was clicked in a port, or a switch popup menu 
  *          if neither of the previous was clicked 
  *          
  * @param MouseEvent e
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void mouseClicked(MouseEvent e)
  {
    /* This method determines what is to be done depending on where
       the mouse was clicked */
    win = JSObject.getWindow(this);
    Unit   curr_unit, prev_unit;
    Interface portobj;
    boolean clickFound = false;
    String dummy="";
    Point clickedPoint = new Point(e.getX(), e.getY());
    dummy = ""+findPortClicked(clickedPoint);

    if (stackFullView)
      curr_unit = (Unit)stack.getFirstUnit();
    else
      curr_unit = (Unit)stack.getUnit(displayedUnit); 

    while (curr_unit != null)
    {
      if (isStacking == 1)
      {
        if (stackFullView == false)
        {
          if (curr_unit.getUnitPrevGet().contains(clickedPoint))
          {
            /*do not do anything if still some click to be processed*/
            if (clickPaint == true)
              return;

            Unit temp_unit;
            int current_view;
            temp_unit = stack.getPrevUnit(curr_unit.getUnitID());
            if (temp_unit != null)
            {
              clickFound = true;
              toggle_in_progress = true;  // Don't allow paint until finished
              displayedUnit = temp_unit.getUnitID();
              deactivateUnit(curr_unit);
              activateUnit(temp_unit);
              toggle_in_progress = false; 
              clickPaint = true;          
            }
          }

          if (curr_unit.getUnitNextGet().contains(clickedPoint))
          {
            /*do not do anything if still some click to be processed*/
            if (clickPaint == true)
            {
              // if (DEBUG)  
              System.out.println("curr_unit.getUnitNextGet().contains(clickedPoint) but clickPaint == true");
              return;
            }

            Unit temp_unit;
            int current_view;
            temp_unit = stack.getNextUnit(curr_unit.getUnitID());
            if (temp_unit != null)
            {
              clickFound = true;
              toggle_in_progress = true;  // Don't allow paint until finished
              displayedUnit = temp_unit.getUnitID();
              deactivateUnit(curr_unit);
              activateUnit(temp_unit);
              toggle_in_progress = false;  
              clickPaint = true;          
            }
          }

          if (stackLink.contains(clickedPoint))
          {
            clickFound = true;
            try
            {
             if (isXUI == 1)
              win.eval("window.open(\"/full_stack.html\",\"_stackApplet\");");
             else
              win.eval("window.open(\"/java/stacking/full_stack.html\",\"_stackApplet\");");

            }
            catch (Throwable t)
            {
              System.out.println("could not open /java/stacking/full_stack.html" + t);
            }
          }

          break;  // Only interested in the displayed unit
        }
        else // Get the next unit
        {
          prev_unit = curr_unit;
          curr_unit = (Unit)stack.getNextUnit(prev_unit.getUnitID());
        }
      }
      else
      {
        break;  // Only one unit for non-stacking systems
      }
    }


    if (clickFound == false)
    {
      switchPopup(clickedPoint.x, clickedPoint.y);
    }
  }

  /*********************************************************************
  *
  * @functions findPortClicked
  *
  * @purpose Finds which port or area was clicked, if any 
  *          
  * @param Point p, where the mouse was clicked
  *
  * @returns String showing the clicked port or clicked area
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public String findPortClicked(Point p)     // TRGTRG Need to change all this code to use the Unit/Card classes to retrieve the objects
  {
    StringTokenizer strtok;
    win = JSObject.getWindow(this);
    String clickedPort="";
    Enumeration enumeration;
    Interface current;
    enumeration=allPorts.elements();
    /* This goes through the entire array of rectangles for base slot 
       in order to determine which slot.port was clicked on */


    while (enumeration.hasMoreElements())
    {
      current=(Interface)enumeration.nextElement();

      // get the dimensions of the current type of port 
      if (current.getBaseRectangle().contains(p))           // if the point is in this rectangle
      {
        if ((current.getStackIntfFlags() & APPLET_IFLAG_STACK_MODE_ONLY) == APPLET_IFLAG_STACK_MODE_ONLY)
        {
          clickedPort="STACKIntf: "+current.getUnit()+"."+current.getSlot()+"."+current.getPort();
        }
        else if ((current.getStackIntfFlags() & APPLET_IFLAG_FPS_SUPPORT) == APPLET_IFLAG_FPS_SUPPORT)
        {
          if ((current.getStackIntfFlags() & APPLET_IFLAG_FPS_MODE_SUPPORT) == APPLET_IFLAG_FPS_MODE_SUPPORT)
            clickedPort="FPS: "+current.getUnit()+"."+current.getSlot()+"."+current.getPort();
          else
            clickedPort="STACKIntf: "+current.getUnit()+"."+current.getSlot()+"."+current.getPort();
        }
        else
        {
          clickedPort="Intf: "+current.getUnit()+"."+current.getSlot()+" Port: "+current.getPort();
        }
        break;
      }
    }

    return(clickedPort);
  }

  /*********************************************************************
  *
  * @functions mousePressed
  *
  * @purpose Not used in this applet, but must be present
  *          
  * @param MouseEvent e
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void mousePressed(MouseEvent e)
  {
  }

  /*********************************************************************
  *
  * @functions mouseReleased
  *
  * @purpose Not used in this applet, but must be present
  *          
  * @param MouseEvent e
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void mouseReleased(MouseEvent e)
  {
  }

  /*********************************************************************
  *
  * @functions mouseEntered
  *
  * @purpose Not used in this applet, but must be present
  *          
  * @param MouseEvent e
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void mouseEntered(MouseEvent e)
  {
  }

  /*********************************************************************
  *
  * @functions mouseExited
  *
  * @purpose Not used in this applet, but must be present
  *          
  * @param MouseEvent e
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  public void mouseExited(MouseEvent e)
  {
  }

  /*********************************************************************
  *
  * @functions actionPerformed
  *
  * @purpose Displays the web page of the menu option that was selected
  *          
  * @param ActionEvent e
  *
  * @returns none
  *
  * @notes A special case exists when extracting the command from the
  * actions array. This is when the command has a WINDOWOPEN prefix.
  * This denotes that the command should be launched using a new window.
  *
  * @end
  *
  *********************************************************************/
  public void actionPerformed(ActionEvent e)
  {
    String WINDOW_OPEN_PREFIX = "WINDOWOPEN";

    /* Determines which command was clicked in the drop-down box and sets
       some hidden html input fields to reflect this data using javascript.
       It then forces the applet to submit. */
    int index=Integer.parseInt(e.getActionCommand());
    String command=actions[index];
    win = JSObject.getWindow(this);

    if (command.startsWith(WINDOW_OPEN_PREFIX,0))
    {
      command = command.replaceAll(WINDOW_OPEN_PREFIX,"");
      try
      {
        /* Launch new window using an equivilent osapiInet_addr method used in tree.html */
        String flags = "status=yes,scrollbars=yes,resizable=yes,menubar=no,toolbar=no,directories=no,location=no,width=700,height=500";
        win.eval("window.open(\"" + command + "\",\"" + Math.abs(InetAddress.getByName(host).hashCode()) + "\",\"" + flags + "\");");
      }
      catch (Throwable t)
      {
        System.out.println("Could not open " + command + " - " + t);
      }
    }
    else
    {
      win.eval("document.forms[0].selPageToLoad.value = \""+ command +"\";");
      if (isXUI == 1)
        win.eval("loadPort()");
      else
        win.eval("document.forms[0].submit();");
    }

  }

  /*********************************************************************
  *
  * @functions portPopup
  *
  * @purpose displays the port popup menu at the coordinates where the 
  *	     mouse was clicked
  *          
  * @param int x
  * @param int y
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  /* shows the popup menu for port area*/
  public void portPopup(int x, int y)
  {
    // if (VERBOSE) System.out.println("Mapapplet popup memu\n");
    portPopupMenu.show(this,x,y);
  }

  public void fpsIntfPopup(int x, int y)
  {
    // if (VERBOSE) System.out.println("Mapapplet popup memu\n");
    fpsIntfPopupMenu.show(this,x,y);
  }

  public void stackIntfPopup(int x, int y)
  {
    // if (VERBOSE) System.out.println("Mapapplet popup memu\n");
    stackIntfPopupMenu.show(this,x,y);
  }


  /*********************************************************************
  *
  * @functions switchPopup
  *
  * @purpose displays the switch popup menu at the coordinates where the 
  *	      mouse was clicked
  *          
  * @param int x
  * @param int y
  *
  * @returns none
  *
  * @notes none
  *
  * @end
  *
  *********************************************************************/
  /* shows the popup menu for outside port area*/
  public void switchPopup(int x, int y)
  {
    switchPopupMenu.show(this,x,y);
    /* Reset the flags that were set when ports menu was popped */
    win = JSObject.getWindow(this);
    win.eval("document.forms[0].intfValid.value = \"0\";");
    win.eval("document.forms[0].stackIntfValid.value = \"0\";");
  }

  /*********************************************************************
  *
  * @functions parseData
  *
  * @purpose Gets the menuItems and actions for the popup menus from the
  *          HTML file
  *          
  * @param String s  - the menu data read in from the PARAMs
  *
  * @returns PopupMenu mainMenu
  *
  * @notes menuItem
  *
  * @end
  *
  *********************************************************************/
  private PopupMenu parseData(String s) 
  {
    // menuItem counters start at -1 so that at first increment, they get set to
    // the first array subscript value of 0
    // menu counters start at 0 so that at first increment, they get set to
    // the array subscript value of 1, the first value (0) being reserved for the main menu
    int levelCtr = -1, menuCtr = 0;// menuItemCtr = -1;
    int levelCount = 0, menuCount = 0, menuItemCount = -1;
    int parentMenuPtr[];
    String itemToken = null, datatoken = null;
    String title = "", action = "", target = "_self";
    boolean newMenu = false;
    if (s == null || s.indexOf("{") == -1)
    {
      //parseError = true;
      //return;
    }
    StringTokenizer braces = new StringTokenizer(s,"{}",true);
    StringTokenizer braceCtr = new StringTokenizer(s,"{}",true);
    StringTokenizer asterisks;
    // Get the number of menus and menuItems for which to allocate array space
    do
    {
      try
      {
        itemToken = braceCtr.nextToken();
      }
      catch (Throwable t)
      {
      }
      if (itemToken.charAt(0) == '{')
      {
        if (newMenu)
          menuCount++;
        newMenu = true;
        levelCtr++;
        if (levelCount < levelCtr) levelCount = levelCtr;
      }
      else if (itemToken.charAt(0) == '}')
      {
        if (newMenu)
          menuItemCount++;
        newMenu = false;
        levelCtr--;
      }
    } while (braceCtr.hasMoreTokens());
    if (levelCtr != -1)
    {
      //parseError = true;
      //return;
    }
    // allocate one more element than the counter values , since the first subscript value is 0

    //actions = new String[menuItemCount+1];
    //targets = new String[menuItemCount+1];
    //menuItems = new MenuItem[menuItemCount+1];
    menus = new Menu[menuCount+1];
    parentMenuPtr = new int[levelCount+1];
    PopupMenu mainMenu = new PopupMenu();
    menus[0] = (Menu)(mainMenu);
    //this.add(mainMenu);
    itemToken = null;
    newMenu = false;
    // Parse the data Param and build the menu and menu items
    do
    {
      try
      {
        itemToken = braces.nextToken();
      }
      catch (Throwable t)
      {
      }
      if (itemToken.charAt(0) == '{')
      {
        if (newMenu)
        {
          menuCtr++;
          menus[menuCtr] = new Menu(title);
          //menus[menuCtr].setFont(menuFont);
          menus[parentMenuPtr[levelCtr]].add(menus[menuCtr]);
          parentMenuPtr[levelCtr+1] = menuCtr;
        }
        newMenu = true;
        levelCtr++;
      }
      else if (itemToken.charAt(0) == '}')
      {
        if (newMenu)
        {
          menuItemCtr++;
          actions[menuItemCtr] = action;
          targets[menuItemCtr] = target;
          menuItems[menuItemCtr] = new MenuItem(title);
          //menuItems[menuItemCtr].setFont(menuFont);
          menuItems[menuItemCtr].addActionListener(this);
          menuItems[menuItemCtr].setActionCommand(new Integer(menuItemCtr).toString());
          menus[parentMenuPtr[levelCtr]].add(menuItems[menuItemCtr]);
        }
        newMenu = false;
        levelCtr--;
      }
      else if (!itemToken.trim().equals(""))
      {
        asterisks = new StringTokenizer(itemToken,"*");
        try
        {
          title = asterisks.nextToken();
          // a menu separator is a -, but allow for hr as well, as in HTML syntax
          if (title.equals("-") || title.equalsIgnoreCase("HR"))
            title = "-";
        }
        catch (Throwable t)
        {
          title = "-";
        }
        try
        {
          action = asterisks.nextToken();
        }
        catch (Throwable t)
        {
          action = "";
        }
        try
        {
          target = asterisks.nextToken();
        }
        catch (Throwable t)
        {
          target = "_self";
        }
      }
    } while (braces.hasMoreTokens());

    return(mainMenu);
  }
}

