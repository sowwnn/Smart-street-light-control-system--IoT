import streamlit as st
import paho.mqtt.client as paho
import json
import pandas as pd
import plotly.graph_objects as go
import plotly.express as px
import  streamlit_toggle as tog


df = pd.DataFrame()
print(df)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_message(client, userdata, msg):
    global df
    global pl1
    global pl2
    global col1,col2,col3
    # print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload)) 
    decoded_message=str(msg.payload.decode("utf-8")) 
    if "Water" in decoded_message:
        mess= json.loads(decoded_message)
        col1.metric("Temperature", f"{mess['Temp']} °F")
        col2.metric("Rain", f"{mess['Water']} mm3")
        col3.metric("Humidity", f"{mess['Humid'] }%")
        mess= pd.DataFrame(mess,index=[0])
    
        df = df.append(mess,ignore_index=True)
        pl1.line_chart(df[["Water"]])
        pl2.line_chart(df[["Humid","Temp"]])
        df.to_csv("data.csv")
    


def client():
    client = paho.Client()
    client.username_pw_set('Sho', '1234')
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    client.connect('broker.mqttdashboard.com', 1883)
    client.subscribe('sho/temp', qos=1)
    return client
    
    # print("Not")

st.set_page_config(
    page_title="Octopuses-Dashboard",
    page_icon="🐙",
    initial_sidebar_state="expanded",
)

st.header("🐙 Octopuses - Application")

st.subheader("Mode")
mode = st.select_slider("", ["Control", "Auto"])
if mode == "Auto":
    col1, col2, col3 = st.columns(3)
    with col1:
        col1 = st.empty()
    with col2:
        col2 = st.empty()
    with col3:
        col3 = st.empty()
    st.subheader("Water")
    pl1 = st.empty()
    st.subheader("Humidity and Tempurature")
    pl2 = st.empty()
    clt = client()
    clt.publish('sho/temp','Mode:auto')
    while True:
        clt.loop_forever()
 
else:
    select = st.multiselect("Choose the light: ",["1","2","3",'all'])
    level = st.select_slider("Level: ",[0,50,255])
    mess = {
        "Mode":"control",
        "Led":select,
        "Level": level,
    }
    # st.write(select)
    clt = client()
    if st.button('Push'):
        clt.publish("sho/temp",str(mess))



