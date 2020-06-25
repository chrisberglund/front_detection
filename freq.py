import os
import pandas as pd

def front_frequency():
    cwd = os.getcwd()
    include_months = ['06','07','08','09','10','11']
    if not os.path.exists(cwd + "/freq"):
        os.makedirs(cwd + "/freq")
    for year in range(2003, 2019):
        directory = "./out/" + str(year) + "/"
        frequencies = pd.read_csv("./out/"+ str(year) +"/" + os.listdir(directory)[0])
        frequencies["Count"] = 1
        files = os.listdir(directory)
        files = [files[1:][i * 25:(i + 1) * 25] for i in range((len(files[1:]) + 25 - 1) // 25)]
        for batch in files:
            df_list = [frequencies]
            for file in batch:
                if any(x in file[5:7] for x in include_months):
                    print(file)
                    df = pd.read_csv("./out/" + str(year) +"/" +  file)
                    df["Count"] = 1
                    df_list.append(df)
            frequencies = pd.concat(df_list)
            frequencies = frequencies.groupby(["Latitude", "Longitude"]).sum().reset_index()

        frequencies["Freq"] = frequencies["Data"]/frequencies["Count"]
        frequencies = frequencies[frequencies["Count"] >= 18]
        print("Writing: %s " %(year) )
        frequencies.to_csv(cwd + "/freq/"+ str(year) +".csv",index=False)



def main():
    front_frequency()


if __name__ == "__main__":
    main()