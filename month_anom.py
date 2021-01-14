import pandas as pd
from pathlib import Path

def main():
    path = Path("anomaly/")
    dfs = []
    for year in range(2003, 2018):
        print(year)
        for month in range(6, 12):
            df = pd.concat([pd.read_csv(p) for p in path.rglob(f"{year}_{month}*")])
            df["Count"] = 1
            df = df.groupby(["Latitude", "Longitude"]).sum()
            dfs.append(df)
        df = pd.concat(dfs)
        df["Data"] = df["Data"]/df["Count"]
        df = df.drop("Count", axis=1).reset_index()
        df.to_csv(f"annual_anomaly/{year}.csv",index=False)

if __name__ == "__main__":
    main()