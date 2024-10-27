import requests
from bs4 import BeautifulSoup
import csv
import re

# Step 1: Fetch the web page
url = 'https://www.timeanddate.com/sun/canada/victoria'
response = requests.get(url)
soup = BeautifulSoup(response.text, 'html.parser')

# Step 2: Find all table rows containing the 'Click to expand for more details' in their 'title' attribute
rows = soup.find_all('tr', title="Click to expand for more details")

# Step 3: Create a list to hold extracted data
table_data = []

# Function to clean symbols from a string (e.g., remove arrows, degrees, parentheses content)
def clean_text(text):
    # Remove any arrows (↑), degree symbols (°), and content inside parentheses (and the parentheses)
    cleaned_text = re.sub(r'[↑°]', '', text)  # Remove ↑ and °
    cleaned_text = re.sub(r'\(.*?\)', '', cleaned_text)  # Remove text within parentheses
    return cleaned_text.strip()  # Remove extra spaces

# Step 4: Extract the required columns from each row
for row in rows:
    columns = row.find_all(['th', 'td'])  # Get all <th> and <td> elements
    day = columns[0].text.strip()  # Day of the month (1-31)
    sunrise = clean_text(columns[1].text.strip())
    sunset = clean_text(columns[2].text.strip())
    length = columns[3].text.strip()
    diff = columns[4].text.strip()
    start_twilight = columns[5].text.strip()
    end_twilight = columns[6].text.strip()
    start_night = columns[7].text.strip()
    end_night = columns[8].text.strip()
    start_dawn = columns[9].text.strip()
    end_dawn = columns[10].text.strip()
    solar_noon = clean_text(columns[11].text.strip())
    distance = columns[12].text.strip()
    
    # Append the cleaned data as a row
    table_data.append([day, sunrise, sunset, length, diff, start_twilight, end_twilight, start_night, end_night, start_dawn, end_dawn, solar_noon, distance])

# Step 5: Write the cleaned data to a CSV file
with open('sun_data.csv', mode='w', newline='', encoding='utf-8') as file:
    writer = csv.writer(file)
    
    # Write the header
    writer.writerow(['Day', 'Sunrise', 'Sunset', 'Length', 'Diff.', 'Start Twilight', 'End Twilight', 'Start Night', 'End Night', 'Start Dawn', 'End Dawn', 'Solar Noon', 'Distance (Mil. km)'])
    
    # Write the table rows
    writer.writerows(table_data)

print("CSV file 'sun_data_cleaned.csv' has been created with cleaned data.")
