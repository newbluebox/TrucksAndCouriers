import java.io.*;

public class TrucksAndCouriers {
  public String[] planShipping(int truckFixed, int truckVariable,
                   int[] warehouseX, int[] warehouseY, int[] warehouseItem, int[] warehouseQuantity,
                   int[] customerX, int[] customerY, int[] customerItem) {
    return new String[0];
  }
  
    private static int[] readArray(BufferedReader br) throws Exception {
      int[] ret = new int[Integer.parseInt(br.readLine())];
      for (int i = 0; i < ret.length; i++) ret[i] = Integer.parseInt(br.readLine());
      return ret;
    }
  
    public static void main(String[] args) {
        try {
		    TrucksAndCouriers sol = new TrucksAndCouriers();
            BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
            int truckFixed = Integer.parseInt(br.readLine());
            int truckVariable = Integer.parseInt(br.readLine());
            int[] warehouseX = readArray(br);
            int[] warehouseY = readArray(br);
            int[] warehouseItem = readArray(br);
            int[] warehouseQuantity = readArray(br);
            int[] customerX = readArray(br);
            int[] customerY = readArray(br);
            int[] customerItem = readArray(br);
            String[] ret = sol.planShipping(truckFixed, truckVariable, warehouseX, warehouseY,
                             warehouseItem, warehouseQuantity, customerX, customerY, customerItem);
            System.out.println(ret.length);
            for (int i = 0; i < ret.length; i++) System.out.println(ret[i]);
            System.out.flush();
        } catch (Exception e) {
        }
    }
}