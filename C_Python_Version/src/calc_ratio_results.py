 #!/usr/bin/env python3
# chmod u+x
 
import sys
import re
from decimal import *

def main():
  if len(sys.argv) < 2:
    print("Usage<path result_LP> to calculate the ratio")
    sys.exit(1)

  file_path = sys.argv[1]
  getcontext().prec = 6

  try:
    with open(file_path,"r") as file:
      total_Integral = 0
      total_Continuos = 0
      lines = file.readlines()
      continuos_value = 0
      integral_value  = 0
      max_ratio = 1
      for line_index in range(len(lines)) :
        line = lines[line_index]
        isIntegral = True
        if line.find("Continuos") >=0:
          isIntegral = False

        splited_line = line.split("Total Cost:")
        if len(splited_line) >1:

          splited_line[1] = splited_line[1].strip() # for spaces
          splited_line[1] = splited_line[1].strip('\n') # for the new line
          cost = float(splited_line[1].split()[0])
          if isIntegral:
            total_Integral += cost
            integral_value = cost
            if ( float( Decimal(continuos_value) / Decimal(integral_value) )) <= max_ratio:
              max_ratio = float( Decimal(continuos_value) / Decimal(integral_value))
          else:
            total_Continuos += cost
            continuos_value = cost

      file.close()

    file_path_splt = file_path.split(".txt")
    file_out = file_path_splt[0] + "_ration.txt"

    with open(file_out,"w") as file:
      file.write(str(float( Decimal(total_Continuos) / Decimal(total_Integral) ) ) +"\n")
      file.write(str(max_ratio))
      file.close()

  except Exception as e:
    print("Error opening file " + file_path)
    raise e

if __name__ == '__main__':
  main()



