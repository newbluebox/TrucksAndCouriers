import java.io.*;
import java.security.*;
import java.util.*;

public class TrucksAndCouriersVis {
  String failMessage = "";
  long totalCost = 0;
  int truckFixed, truckVariable, numWarehouses, numItems, numCustomers;
  int[] customerX, customerY, customerItem;
  int[] warehouseX, warehouseY, warehouseItem, warehouseQuantity;
  int[][][] warehouses = new int[1001][1001][100];
  int[][][] customers = new int[1001][1001][100];
  
  public String displayTestCase(String s) {
    generateTestCase(Long.parseLong(s));
    String ret = "";
    ret += "Truck cost: fixed = " + truckFixed + ", variable = " + truckVariable + "\n";
    ret += "Number of Warehouses = " + numWarehouses + "\n";
    ret += "Number of Items = " + numItems + "\n";
    ret += "Number of Customers = " + numCustomers + "\n";
    return ret;
  }
  
  private void generateTestCase(long seed) {
    Random r = new Random(seed);
    try {
      SecureRandom sr = SecureRandom.getInstance("SHA1PRNG"); 
      sr.setSeed(seed);
      r = sr;
    } catch (NoSuchAlgorithmException ex) {}
	truckFixed = r.nextInt(46) + 5;
	truckVariable = r.nextInt(20) + 1;
    numItems = r.nextInt(91) + 10;
	numCustomers = r.nextInt(981) + 20;
	customerX = new int[numCustomers];
	customerY = new int[numCustomers];
	customerItem = new int[numCustomers];
	int[] itemCount = new int[numItems];
	for (int i = 0; i < numCustomers; i++) {
	  customerX[i] = r.nextInt(1001);
	  customerY[i] = r.nextInt(1001);
	  itemCount[customerItem[i] = r.nextInt(numItems)]++;
	  customers[customerX[i]][customerY[i]][customerItem[i]]++;
	}
	numWarehouses = r.nextInt(18) + 3;
	int[] wx = new int[numWarehouses];
	int[] wy = new int[numWarehouses];
	for (int i = 0; i < numWarehouses; i++) {
	  wx[i] = r.nextInt(1001);
	  wy[i] = r.nextInt(1001);
	}
	ArrayList<Integer> wwx = new ArrayList<Integer>();
	ArrayList<Integer> wwy = new ArrayList<Integer>();
	ArrayList<Integer> wwi = new ArrayList<Integer>();
	ArrayList<Integer> wwq = new ArrayList<Integer>();
	for (int i = 0; i < numItems; i++) {
	  if (itemCount[i] == 0) continue;
      itemCount[i] += r.nextInt(1 + itemCount[i] / 2);
	  int[] w = new int[3];
      w[0] = r.nextInt(numWarehouses);
      w[1] = r.nextInt(numWarehouses);
      w[2] = r.nextInt(numWarehouses);
	  int[] c = new int[3];
	  for (int j = 0; j < itemCount[i]; j++)
	    c[r.nextInt(3)]++;
	  for (int j = 0; j < 3; j++) {
	    if (c[j] == 0) continue;
		wwx.add(wx[w[j]]);
		wwy.add(wy[w[j]]);
		wwi.add(i);
		wwq.add(c[j]);
		warehouses[wx[w[j]]][wy[w[j]]][i] += c[j];
	  }
	}
	warehouseX = new int[wwx.size()];
	warehouseY = new int[wwx.size()];
	warehouseItem = new int[wwx.size()];
	warehouseQuantity = new int[wwx.size()];
	for (int i = 0; i < wwx.size(); i++) {
	  warehouseX[i] = wwx.get(i);
	  warehouseY[i] = wwy.get(i);
	  warehouseItem[i] = wwi.get(i);
	  warehouseQuantity[i] = wwq.get(i);
	}
  }
  
  private boolean checkXY(int startX, int startY, int destX, int destY) {
    if (startX < 0 || startX > 1000) {
      failMessage += "Invalid startX: " + startX + "\n";
      return false;
    }
    if (startY < 0 || startY > 1000) {
      failMessage += "Invalid startY: " + startY + "\n";
      return false;
    }
    if (destX < 0 || destX > 1000) {
      failMessage += "Invalid destX: " + destX + "\n";
      return false;
    }
    if (destY < 0 || destY > 1000) {
      failMessage += "Invalid destY: " + destY + "\n";
      return false;
    }
    return true;
  }

  private boolean checkItem(int itemNumber) {
    if (itemNumber < 0 || itemNumber >= numItems) {
      failMessage += "Invalid item number: " + itemNumber + "\n";
      return false;
    }
    return true;
  }  
  
  private boolean truck(int startX, int startY, int destX, int destY, int[] items) {
    if (!checkXY(startX, startY, destX, destY))
      return false;
    for (int i = 0; i < items.length; i++) {
      if (!checkItem(items[i]))
        return false;
      if (warehouses[startX][startY][items[i]] <= 0) {
	    failMessage += "(" + startX + "," + startY + ") does not have any of item " + items[i] + " to ship by truck.\n";
	    return false;
	  }
  	  warehouses[startX][startY][items[i]]--;
	  warehouses[destX][destY][items[i]]++;
	}    
	totalCost += truckFixed + truckVariable * (Math.abs(destX - startX) + Math.abs(destY - startY));
    return true;
  }

  private boolean courier(int startX, int startY, int destX, int destY, int item) {
    if (!checkXY(startX, startY, destX, destY))
      return false;
    if (!checkItem(item))
      return false;
    if (warehouses[startX][startY][item] <= 0) {
	  failMessage += "(" + startX + "," + startY + ") does not have any of item " + item + " to courier.\n";
	  return false;
	}
    if (customers[destX][destY][item] <= 0) {
	  failMessage += "(" + destX + "," + destY + ") does not have any customers waiting for item " + item + ".\n";
	  return false;
	}
	warehouses[startX][startY][item]--;
	customers[destX][destY][item]--;
	totalCost += Math.abs(destX - startX) + Math.abs(destY - startY);
    return true;
  }

private void writeOutput(String s) {
  if (!vis) return;
  System.out.println(s);
}
  
  public double runTest(String testValue) {
    generateTestCase(Long.parseLong(testValue));
	int timeLeft = 10000;
    long start = System.currentTimeMillis();
    String[] ret;
    try {
	  ret = planShipping(truckFixed, truckVariable,
	                warehouseX, warehouseY, warehouseItem, warehouseQuantity,
					customerX, customerY, customerItem);
    } catch (Exception e) {
      writeOutput("Error calling planShipping()");
      return -1;
    }
    long end = System.currentTimeMillis();
    long elapsed = end - start;
    timeLeft -= elapsed;
    if (timeLeft < 10) {
      writeOutput("Time limit exceeded.");
      return -1;
    }
    for (String s : ret) {
      String[] t = s.split(",");
      if (!t[0].equals("C") && !t[0].equals("T")) {
        writeOutput("Invalid shipment type: " + t[0] + "\n");
        return -1;
      }
      if (t.length < 6) {
        writeOutput("No items specified in shipment: " + s + "\n");
        return -1;
      }
      if (t[0].equals("C") && t.length > 6) {
        writeOutput("Cannot ship multiple items with a courier: " + s + "\n");
        return -1;
      }
      int startX, startY, endX, endY;
      int[] itemList = new int[t.length - 5];
      try {
        startX = Integer.parseInt(t[1]);
        startY = Integer.parseInt(t[2]);
        endX = Integer.parseInt(t[3]);
        endY = Integer.parseInt(t[4]);
        for (int i = 0; i < itemList.length; i++)
          itemList[i] = Integer.parseInt(t[i + 5]);
      } catch (NumberFormatException ex) {
        writeOutput("Invalid number format found: " + s + "\n");
        return -1;
      }
      boolean res = t[0].equals("C") ? courier(startX, startY, endX, endY, itemList[0]) :
                                       truck(startX, startY, endX, endY, itemList);
      if (!res) {
        writeOutput(failMessage);
        return -1;
      }
    }
	if (failMessage.length() > 0) {
	  writeOutput(failMessage);
	  return -1;
	}
	for (int i = 0; i <= 1000; i++) for (int j = 0; j <= 1000; j++) for (int k = 0; k < 100; k++)
	  if (customers[i][j][k] > 0) totalCost += customers[i][j][k] * 10000;
	return totalCost;
  }


// ------------- visualization part ------------
static String exec;
static boolean vis, debug;
static Process proc;
InputStream is;
OutputStream os;
BufferedReader br;

private void appendArray(StringBuffer sb, int[] a) {
  //System.out.println( a.length );
  sb.append(a.length).append("\n");
  for (int i = 0; i < a.length; i++) sb.append(a[i]).append("\n");
}

private String[] planShipping(int truckFixed, int truckVariable,
                   int[] warehouseX, int[] warehouseY, int[] warehouseItem, int[] warehouseQuantity,
                   int[] customerX, int[] customerY, int[] customerItem)  throws IOException {
  StringBuffer sb = new StringBuffer();
  sb.append(truckFixed).append("\n");
  sb.append(truckVariable).append("\n");
  appendArray(sb, warehouseX);
  appendArray(sb, warehouseY);
  appendArray(sb, warehouseItem);
  appendArray(sb, warehouseQuantity);
  appendArray(sb, customerX);
  appendArray(sb, customerY);
  appendArray(sb, customerItem);
  os.write(sb.toString().getBytes());
  os.flush();
  String[] ret = new String[Integer.parseInt(br.readLine())];
  for (int i = 0; i < ret.length; i++) ret[i] = br.readLine();
  return ret;
}

    public TrucksAndCouriersVis(String seed) {
      try {
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                new ErrorReader(proc.getErrorStream()).start();
            } catch (Exception e) { e.printStackTrace(); }
        }
        System.out.println("Score = " + runTest(seed));
        if (proc != null)
            try { proc.destroy(); } 
            catch (Exception e) { e.printStackTrace(); }
      }
      catch (Exception e) { e.printStackTrace(); }
    }
    // -----------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        vis = true;
        for (int i = 0; i<args.length; i++)
        {   if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-novis"))
                vis = false;
            if (args[i].equals("-debug"))
                debug = true;
        }

        TrucksAndCouriersVis f = new TrucksAndCouriersVis(seed);
    }
}

class ErrorReader extends Thread{
    InputStream error;
    public ErrorReader(InputStream is) {
        error = is;
    }
    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0)
            {   String s = new String(ch,0,read);
                System.out.print(s);
                System.out.flush();
            }
        } catch(Exception e) { }
    }
}
